#include "kernel.hpp"

#include "../../boot/multiboot2.hpp"
#include "../desktop/desktop.hpp"
#include "../gui/window_app_registry.hpp"
#include "../gui/window_manager.hpp"
#include "../memory/heap.hpp"
// #include "../process/process_manager.hpp"
// #include "../process/scheduler.hpp"
#include "../random/random.hpp"
#include "../shared/types.hpp"
// #include "../shell/shell.hpp"
#include "../drivers/mouse.hpp"
#include "../shell/terminal.hpp"
#include "../shell/terminal_registry.hpp"
#include "../utils/debugger.hpp"

#include "crti.hpp"
#include "timer.hpp"

extern "C" void test_process_1() {
  Debugger::log(Debugger::DebugType::PROCESS, "P1 ENTERED\n");

  volatile u32 counter = 0;

  while (true) {
    counter += 1;
  }
}

extern "C" void test_process_2() {
  Debugger::log(Debugger::DebugType::PROCESS, "P2 ENTERED\n");

  volatile u32 counter = 0;

  while (true) {
    counter += 1;
  }
}

extern "C" void test_process_3() {
  Debugger::log(Debugger::DebugType::PROCESS, "P3 ENTERED\n");

  volatile u32 counter = 0;

  while (true) {
    counter += 1;
  }
}

/*
 * Fault handlers, see loader.s pagefault_stub / gpfault_stub.
 *
 * saved_regs points at the 15 GPRs pushed by the stub:
 *
 * rax, rcx, rdx, rbx, rbp, rsi, rdi, r8-r15
 *
 * The CPU's own error code sits immediately above them at index 15.
 */
extern "C" void c_pagefault_handler(u64 *saved_regs) {
  Debugger::log(Debugger::DebugType::KERNEL, "PAGE FAULT!\n");

  u64 fault_addr;

  asm volatile("mov %%cr2, %0" : "=r"(fault_addr));

  u64 error_code = saved_regs[15];
  u64 fault_rip = saved_regs[16];

  Debugger::logf(Debugger::DebugType::KERNEL, "PAGE FAULT at addr=%x err=%x\n",
                 (unsigned)fault_addr, (unsigned)error_code);

  Debugger::logf(Debugger::DebugType::KERNEL, "present=%d write=%d user=%d\n",
                 (int)(error_code & 1), (int)((error_code >> 1) & 1),
                 (int)((error_code >> 2) & 1));

  Debugger::logf(Debugger::DebugType::KERNEL, "faulting RIP=%x\n",
                 (unsigned)fault_rip);
}

extern "C" void c_gpfault_handler(u64 *saved_regs) {
  Debugger::log(Debugger::DebugType::KERNEL, "GP FAULT!\n");

  u64 error_code = saved_regs[15];

  Debugger::logf(Debugger::DebugType::KERNEL, "GP FAULT err=%x\n",
                 (unsigned)error_code);
}

extern "C" void kernel_main(u64 mb_addr) {
  call_constructors();

  /*
   * The framebuffer must be discovered before the debugger
   * attempts to draw anything.
   */
  Multiboot2::fill_tags(mb_addr);

  Debugger::log(Debugger::DebugType::KERNEL, "KERNEL MAIN ENTERED\n");

  heap.init_heap();

  Timer::init();
  Random::init();

  Mouse::init();

  Mouse::set_coords(Multiboot2::framebuffer.width / 2,
                    Multiboot2::framebuffer.height / 2);

  Asm::sti();

  /*
   * Filesystem
   */
  Disk disk;
  FileSystem fs(disk);

  if (!fs.mount()) {
    Debugger::log(Debugger::DebugType::FS, "No filesystem, formatting...\n");

    if (!fs.format()) {
      Debugger::log(Debugger::DebugType::FS,
                    "FORMAT FAILED - filesystem commands will not work\n");
    }
  } else {
    Debugger::log(Debugger::DebugType::FS, "MOUNT SUCCESS\n");
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

  ShellHistory shell_history;
  Shell shell(terminal, shell_history);

  /*
   * GUI test.
   *
   * This block takes over completely and never returns.
   * Shell/process testing below therefore remains unreachable
   * while the GUI is active.
   */


  WindowManager wm;
  WindowAppRegistry window_app_reg(terminal, shell);

  window_app_reg.fill_registry();

  DialogManager dialog_manager;

  AppLauncher app_launcher(wm, window_app_reg, dialog_manager);

  Desktop desktop(wm, window_app_reg, dialog_manager, app_launcher);

  const char *app_names[] = {
      "Counter",  "Dice",      "DVD",      "Clock", "Tyrant",  "Matrix",
      "About",    "Settings",

      "Rock AI",  "Files",     "Terminal", "Amp",   "Tuner",   "Metronome",
      "Playlist", "Radio",     "Lyrics",   "Mixer", "Browser", "Rock Store",
      "Vinyl",    "Recorder",  "Drums",    "REC",   "Lock",    "Trash",
      "Updater",  "Equalizer", "Pick",     "Help",  "Stage"};

  constexpr u32 START_X = 20;
  constexpr u32 START_Y = 20;
  constexpr u32 SPACING = 70;
  constexpr u32 MAX_X = 630;

  u32 x = START_X;
  u32 y = START_Y;

  for (const char *name : app_names) {
    desktop.add_icon(name, x, y);

    x += SPACING;

    if (x >= MAX_X) {
      x = START_X;
      y += SPACING;
    }
  }

  desktop.init();

  desktop.run();

  /*
   * Process system
   */
  /*
    constexpr u32 TEST_MEMORY = 128 * 1024 * 1024;

    FrameAllocator frame_allocator(TEST_MEMORY);

    PageTable::debug_kernel_pml4();

    ProcessManager process_manager(frame_allocator);
    Scheduler scheduler(process_manager);

    Asm::cli();

    Process *p1 =
        process_manager.create_process(64 * 1024, test_process_1).p;

    if (!p1) {
      Debugger::log(
          Debugger::DebugType::PROCESS,
          "P1 CREATE FAILED\n"
      );

      while (true)
        Kernel::halt();
    }

    Debugger::log(
        Debugger::DebugType::PROCESS,
        "P1 CREATED\n"
    );

    Process *p2 =
        process_manager.create_process(64 * 1024, test_process_2).p;

    if (!p2) {
      Debugger::log(
          Debugger::DebugType::PROCESS,
          "P2 CREATE FAILED\n"
      );

      while (true)
        Kernel::halt();
    }

    Debugger::log(
        Debugger::DebugType::PROCESS,
        "P2 CREATED\n"
    );

    Process *p3 =
        process_manager.create_process(64 * 1024, test_process_3).p;

    if (!p3) {
      Debugger::log(
          Debugger::DebugType::PROCESS,
          "P3 CREATE FAILED\n"
      );

      while (true)
        Kernel::halt();
    }

    Debugger::log(
        Debugger::DebugType::PROCESS,
        "P3 CREATED\n"
    );

    Asm::sti();

    while (true)
      Kernel::halt();
  */

  /*
   * Shell
   */
  /*
    ShellHistory sh;
    Shell shell(terminal, sh);
    shell.run();
  */
}
