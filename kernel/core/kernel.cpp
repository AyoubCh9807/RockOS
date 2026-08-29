#include "kernel.hpp"
#include "../../boot/graphics.hpp"
#include "../../boot/multiboot2.hpp"
#include "../gui/demo_app.hpp"
#include "../gui/clock_app.hpp"
#include "../gui/tyrant_app.hpp"
#include "../gui/window.hpp"
#include "../gui/window_app.hpp"
#include "../gui/window_manager.hpp"
#include "../memory/heap.hpp"
#include "../process/process_manager.hpp"
#include "../process/scheduler.hpp"
#include "../random/random.hpp"
#include "../shared/types.hpp"
#include "../shell/shell.hpp"
#include "../shell/terminal.hpp"
#include "../shell/terminal_registry.hpp"
#include "crti.hpp"
#include "timer.hpp"

extern "C" void test_process_1() {
  Debugger::log("P1 ENTERED\n");
  volatile u32 counter = 0;
  while (true) {
    counter++;
  }
}

extern "C" void test_process_2() {
  Debugger::log("P2 ENTERED\n");
  volatile u32 counter = 0;
  while (true) {
    counter++;
  }
}

extern "C" void test_process_3() {
  Debugger::log("P3 ENTERED\n");
  volatile u32 counter = 0;
  while (true) {
    counter++;
  }
}

/* Fault handlers, see loader.s pagefault_stub / gpfault_stub.
   saved_regs points at the 15 GPRs pushed by the stub (rax, rcx, rdx,
   rbx, rbp, rsi, rdi, r8-r15, in that push order), so the CPU's own
   error code sits immediately above them at index 15. */
extern "C" void c_pagefault_handler(u64 *saved_regs) {
  Debugger::log("PAGE FAULT!\n");
  u64 fault_addr;
  asm volatile("mov %%cr2, %0" : "=r"(fault_addr));
  u64 error_code = saved_regs[15];
  u64 fault_rip = saved_regs[16];
  Debugger::logf("PAGE FAULT at addr=%d err=%d\n", (unsigned)fault_addr,
                 (unsigned)error_code);
  Debugger::logf("  present=%d write=%d user=%d\n", (int)(error_code & 1),
                 (int)((error_code >> 1) & 1), (int)((error_code >> 2) & 1));
  Debugger::logf("  faulting RIP=%d\n", (unsigned)fault_rip);
}

extern "C" void c_gpfault_handler(u64 *saved_regs) {
  Debugger::log("GP FAULT!\n");
  u64 error_code = saved_regs[15];
  Debugger::logf("GP FAULT err=%x\n", (unsigned)error_code);
}

extern "C" void kernel_main(u64 mb_addr) {
  call_constructors();
  Debugger::log("KERNEL MAIN ENTERED\n");
  heap.init_heap();
  Timer::init();
  Random::init();
  Multiboot2::fill_tags(mb_addr);

  /*
   * Filesystem
   */
  Disk disk;
  FileSystem fs(disk);
  if (!fs.mount()) {
    Debugger::log("No filesystem, formatting...\n");
    if (!fs.format()) {
      Debugger::log("FORMAT FAILED - filesystem commands will not work\n");
    }
  } else {
    Debugger::log("MOUNT SUCCESS\n");
  }

  /*
   * Shell
   */
  u32 current_dir = ROOT_INODE;
  TerminalUtils terminal_utils;
  Environment env(terminal_utils);
  TerminalRegistry terminal_registry(terminal_utils, fs, current_dir, env);
  CliAppRegistry cli_app_registry(terminal_utils);
  Terminal terminal(terminal_utils, fs, terminal_registry, cli_app_registry,
                    env);
  terminal.fill_registry();
  terminal_utils.print(Generator::random_phrase(reboot_phrases), 0xFFFFFF);

  /*
   * GUI test. Same pattern as the process test below, this block
   * takes over completely and never returns, so shell.run() (and the
   * process test, if that were also uncommented) becomes unreachable
   * while this is active. Flip this and the shell block below to
   * switch between testing the GUI and testing the shell, they are
   * not set up to coexist yet.
   */
  
    WindowManager wm;
    TyrantApp tyrant;
    wm.create_window(&tyrant, 100, 100, 600, 480);
    wm.run();
  

  /*
   * Process system
   */
  /*  constexpr u32 TEST_MEMORY = 128 * 1024 * 1024;
    FrameAllocator frame_allocator(TEST_MEMORY);
    PageTable::debug_kernel_pml4();
    ProcessManager process_manager(frame_allocator);
    Scheduler scheduler(process_manager);

    Asm::cli(); // no preemption while we set up processes

    Process *p1 = process_manager.create_process(64 * 1024, test_process_1).p;
    if (!p1) {
      Debugger::log("P1 CREATE FAILED\n");
      while (true)
        Kernel::halt();
    }
    Debugger::log("P1 CREATED\n");

    Process *p2 = process_manager.create_process(64 * 1024, test_process_2).p;
    if (!p2) {
      Debugger::log("P2 CREATE FAILED\n");
      while (true)
        Kernel::halt();
    }
    Debugger::log("P2 CREATED\n");

    Process *p3 = process_manager.create_process(64 * 1024, test_process_3).p;
  if (!p3) {
    Debugger::log("P3 CREATE FAILED\n");
    while (true)
      Kernel::halt();
  }
  Debugger::log("P3 CREATED\n");

    Asm::sti(); // safe now, both processes exist before the timer can act

    while (1)
      Kernel::halt();
      */

  /*
   * Shell (unreachable while the GUI test or process test above are
   * uncommented, kept here so it is easy to switch back)
   */
  ShellHistory sh;
  Shell shell(terminal, sh);
  shell.run();
}
