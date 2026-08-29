#pragma once

#include "../containers/string.hpp"
#include "asm.hpp"
#include "idt.hpp"

constexpr int DIVISOR = 11932;
constexpr int TIMER_HZ = 100;

/* Scheduler::on_timer sets these before returning. timer_stub loads
   them into the real CR3 and RSP registers, back to back, right
   before popping registers and executing iretq. This is what makes
   each process resume on its own private stack and page tables
   instead of whatever was active when the interrupt fired. */
extern "C" inline volatile u64 next_resume_rsp = 0;
extern "C" inline volatile u64 next_resume_cr3 = 0;

namespace Timer {

volatile inline unsigned int ticks = 0;
volatile inline unsigned int idle_ticks = 0;
inline bool cpu_idle;

volatile inline bool bar_dirty = false;
volatile inline int last_bar_second = -1;

inline void remap_pic() {
  // Save masks (not necessary but good)
  unsigned char a1 = Asm::inb(0x21);
  unsigned char a2 = Asm::inb(0xA1);

  /* An ICW (initialization command word) is a setting we send to the
     PIC since we cannot write normal settings. ICW1 starts
     initialization. */
  Asm::outb(0x20, 0x11);
  Asm::outb(0xA0, 0x11);

  /* ICW2 delivers offsets (master = 32, slave = 40). We do that
     because vectors 0 through 31 handle exceptions such as divide by
     zero or faults. */
  Asm::outb(0x21, 0x20);
  Asm::outb(0xA1, 0x28);

  // ICW3 sets up cascading, how the two PICs talk to each other.
  Asm::outb(0x21, 0x04);
  Asm::outb(0xA1, 0x02);

  // ICW4 sets environment info.
  Asm::outb(0x21, 0x01);
  Asm::outb(0xA1, 0x01);

  /* Mask every IRQ except IRQ0 (timer). Keyboard is polled directly
     in kernel_main rather than handled via IRQ1, and no other line
     has a handler yet, so keep them masked to avoid faulting on an
     unhandled vector when the hardware fires them. */
  Asm::outb(0x21, 0xFC); // Master: 1111 1110, only IRQ0 unmasked
  Asm::outb(0xA1, 0xFF); // Slave: all masked
}

inline void init() {
  remap_pic();

  // Set up the Interrupt Descriptor Table first so the CPU is ready.
  idt_init();

  /* Preparing to send a 16 bit number on channel zero in 2 parts,
     low byte and then high byte. */
  Asm::outb(command_register_hex, 0x36);

  // Dropping the low byte to the chip.
  Asm::outb(channel_zero_data_port, (unsigned char)(DIVISOR & 0xFF));

  // Dropping the high byte to the chip.
  Asm::outb(channel_zero_data_port, (unsigned char)((DIVISOR >> 8) & 0xFF));

  Asm::sti();
}

inline void handler() {
  ticks++;
  if (cpu_idle)
    idle_ticks++;
}

inline int get_ticks() { return ticks; }

inline int get_seconds() { return ticks / TIMER_HZ; }

inline int get_minutes() { return (ticks / TIMER_HZ) / 60; }

inline int get_hours() { return (ticks / TIMER_HZ) / 3600; }

inline void set_idle(bool val) { cpu_idle = val; }

inline int get_cpu_usage() {
  if (ticks == 0)
    return 0;

  return 100 - ((idle_ticks * 100) / ticks);
}

inline void get_formatted_time_into(char *buf, size_t max_len) {
  int seconds = get_seconds();
  int minutes = get_minutes();
  int hours = get_hours();
  int days = 0;
  if (hours > 24) {
    days = hours / 24;
    hours %= 24;
  }
  seconds %= 60;
  minutes %= 60;
  hours %= 24;

  if (minutes == 0) {
    StringUtils::snprintf(buf, max_len, "Uptime: %d seconds\n", seconds);
  } else if (hours == 0) {
    StringUtils::snprintf(buf, max_len, "Uptime: %d minutes and %d seconds\n",
                          minutes, seconds);
  } else if (days == 0) {
    StringUtils::snprintf(buf, max_len,
                          "Uptime: %d hours, %d minutes and %d seconds\n",
                          hours, minutes, seconds);
  } else {
    StringUtils::snprintf(
        buf, max_len, "Uptime: %d days, %d hours, %d minutes and %d seconds\n",
        days, hours, minutes, seconds);
  }
}

} // namespace Timer

#include "../process/scheduler.hpp"

extern "C" void c_timer_handler(CpuContext *ctx) {
  static u32 hb = 0;
  if ((hb++ % 50) == 0) {
    if (Scheduler::get_scheduler() &&
        Scheduler::get_scheduler()->get_current_process()) {
      int pid =
          (int)Scheduler::get_scheduler()->get_current_process()->get_pid();
      Debugger::logf("HB pid=%d\n", pid);
    }
  }

  next_resume_rsp = reinterpret_cast<u64>(ctx);

  /* Default to the CR3 that is actually loaded right now. Without
     this, next_resume_cr3 stays at its zero-initialized value on any
     tick where on_timer does not run (for example before the
     scheduler exists yet), and timer_stub would force CR3 to 0 on
     iretq. on_timer overwrites this below when a real switch
     happens. */
  next_resume_cr3 = Asm::read_cr3();

  Timer::handler();

  if (Scheduler::get_scheduler())
    Scheduler::get_scheduler()->on_timer(ctx);

  /* Diagnostic: confirm what timer_stub is about to load, on the
     untouched entry stack, safe regardless of what on_timer just did
     to next_resume_cr3 / next_resume_rsp. */
  static u32 resume_dbg = 0;
  if ((resume_dbg++ % 30) == 0)
    Debugger::logf("RESUME cr3=%d rsp=%d\n", (unsigned)next_resume_cr3,
                   (unsigned)next_resume_rsp);
}
