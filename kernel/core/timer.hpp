#pragma once

#include "../containers/string.hpp"
#include "asm.hpp"
#include "idt.hpp"
#include "../process/scheduler.hpp"

constexpr int DIVISOR = 11932;
constexpr int TIMER_HZ = 100;

namespace Timer {

volatile inline unsigned int ticks = 0;
volatile inline unsigned int idle_ticks = 0;
inline bool cpu_idle;

inline void remap_pic() {
  // Save masks (not necessary but good)
  unsigned char a1 = Asm::inb(0x21);
  unsigned char a2 = Asm::inb(0xA1);

  // an ICW (initialization command word) is a setting we send to the PIC since
  // we cant write normal settings ICW1: Start initialization
  Asm::outb(0x20, 0x11);
  Asm::outb(0xA0, 0x11);

  // ICW2: Deliver offsets (Master = 32, Slave = 40)
  // We do that because vector 0 through 31 handle exceptions such as divide by
  // zero or faults
  Asm::outb(0x21, 0x20);
  Asm::outb(0xA1, 0x28);

  // ICW3: Setup cascading
  // cascading is how to PICs talk to each other (pic: programmable interrupt
  // controller)
  Asm::outb(0x21, 0x04);
  Asm::outb(0xA1, 0x02);

  // ICW4: Environment info
  Asm::outb(0x21, 0x01);
  Asm::outb(0xA1, 0x01);

  // Mask every IRQ except IRQ0 (timer). Keyboard is polled directly in
  // kernel_main rather than handled via IRQ1, and no other line has a
  // handler yet, so keep them masked to avoid faulting on an
  // unhandled vector when the hardware fires them.
  Asm::outb(0x21, 0xFC); // Master: 1111 1110 -> only IRQ0 unmasked
  Asm::outb(0xA1, 0xFF); // Slave: all masked
}

inline void init() {

  remap_pic();

  // Set up the Interrup Descriptor Table first so the CPU is ready
  idt_init();

  // Preparing to send a 16 bit number on channel zero in 2 parts: low byte
  // and then high byte
  Asm::outb(command_register_hex, 0x36);

  // Dropping the first byte (low) to the chip and preparing for the second
  // one (high)
  Asm::outb(channel_zero_data_port, (unsigned char)(DIVISOR & 0xFF));

  // Dropping the second byte (high) to the chip
  Asm::outb(channel_zero_data_port, (unsigned char)((DIVISOR >> 8) & 0xFF));

  // Calling sti
  Asm::sti();
}

inline void handler() {
  ticks++;
  if (idle_ticks)
    idle_ticks++;
}

inline int get_ticks() { return ticks; }

inline int get_seconds() { return ticks / TIMER_HZ; }

inline int get_minutes() { return (ticks / TIMER_HZ) / 60; }

inline int get_hours() { return (ticks / TIMER_HZ) / 3600; }

inline String get_formatted_time() {
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
    return String::format("Uptime: %d seconds\n\0", seconds);

  } else if (hours == 0) {
    return String::format("Uptime: %d minutes and %d seconds\n\0", minutes,
                               seconds);
  } else if (days == 0) {
    return String::format(
        "Uptime: %d hours, %d minutes and %d seconds\n\0", hours, minutes,
        seconds);
  } else {
    return String::format(
        "Uptime: %d days, %d hours, %d minutes and %d seconds\n\0", days, hours,
        minutes, seconds);
  }
}

inline void set_idle(bool val) { cpu_idle = val; }

inline int get_cpu_usage() {
  if (ticks == 0)
    return 0;

  return 100 - ((idle_ticks * 100) / ticks);
}

extern "C" void c_timer_handler(CpuContext* ctx) {
  Timer::handler(); 
  Scheduler::on_timer(ctx);
}

} // namespace Timer
