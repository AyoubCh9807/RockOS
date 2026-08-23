#include "kernel.hpp"
#include "../../boot/graphics.hpp"
#include "../../boot/multiboot2.hpp"
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
  while (true) {
    Graphics::draw_string("PROC1 ALIVE", 500, 500, 0x000000);
  }
}

extern "C" void test_process_2() {
  while (true) {
    Graphics::draw_string("2", 200, 100, 0x000000);
  }
}

// Fault handlers - see loader.s pagefault_stub / gpfault_stub.
// saved_regs points at the 15 GPRs pushed by the stub (rax, rcx, rdx,
// rbx, rbp, rsi, rdi, r8-r15, in that push order), so the CPU's own
// error code sits immediately above them at index 15.
extern "C" void c_pagefault_handler(u64 *saved_regs) {
  u64 fault_addr;
  asm volatile("mov %%cr2, %0" : "=r"(fault_addr));

  u64 error_code = saved_regs[15];

  Debugger::logf("PAGE FAULT at addr=%x err=%x\n",
                  (unsigned)fault_addr, (unsigned)error_code);
  // bit 0: 0=not-present, 1=protection violation
  // bit 1: 0=read, 1=write
  // bit 2: 0=kernel, 1=user
  Debugger::logf("  present=%d write=%d user=%d\n",
                  (int)(error_code & 1), (int)((error_code >> 1) & 1),
                  (int)((error_code >> 2) & 1));
}

extern "C" void c_gpfault_handler(u64 *saved_regs) {
  u64 error_code = saved_regs[15];
  Debugger::logf("GP FAULT err=%x\n", (unsigned)error_code);
}

extern "C" void kernel_main(u64 mb_addr) {
  call_constructors();
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

  TerminalRegistry reg(terminal_utils, fs, current_dir, env);

  Terminal terminal(terminal_utils, fs, reg, env);

  terminal.fill_registry();

  Graphics::clear(0x00FF00);

  terminal_utils.print(Generator::random_phrase(reboot_phrases), 0xFFFFFF);

  /*
   * Process system
   */
  constexpr u32 TEST_MEMORY = 128 * 1024 * 1024;

  FrameAllocator frame_allocator(TEST_MEMORY);

  PageTable::debug_kernel_pml4();

  ProcessManager process_manager(frame_allocator);
  Scheduler scheduler(process_manager);

  Process *p1 = process_manager.create_process(64 * 1024, test_process_1).p;

  if (!p1) {
    Debugger::log("PROCESS CREATION FAILED\n");
    Graphics::draw_string("P1 CREATE FAILED", 500, 450, 0x000000);
    while (true)
      Kernel::halt();
  }

  Debugger::log("P1 CREATED\n");
  Graphics::draw_string("P1 CREATE OK", 500, 450, 0x000000);

  /*
   * Shell
   */
  // ShellHistory sh;
  // Shell shell(terminal, sh);
  // shell.run();

  while (1)
    Kernel::halt();
}
