#pragma once

// #include "../process/process_manager.hpp"
// #include "../process/scheduler.hpp"
// #include "../shell/shell.hpp"

/*
 * Process system
 */
/*
 *
 *extern "C" void test_process_1() {
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

*
* Fault handlers, see loader.s pagefault_stub / gpfault_stub.
* saved_regs points at the 15 GPRs pushed by the stub:
* rax, rcx, rdx, rbx, rbp, rsi, rdi, r8-r15
* The CPU's own error code sits immediately above them at index 15.

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
*constexpr u32 TEST_MEMORY = 128 * 1024 * 1024;

FrameAllocator frame_allocator(TEST_MEMORY);

PageTable::debug_kernel_pml4();

ProcessManager process_manager(frame_allocator);
Scheduler scheduler(process_manager);

Asm::cli();

Process *p1 = process_manager.create_process(64 * 1024, test_process_1).p;

if (!p1) {
  Debugger::log(Debugger::DebugType::PROCESS, "P1 CREATE FAILED\n");

  while (true)
    Kernel::halt();
}

Debugger::log(Debugger::DebugType::PROCESS, "P1 CREATED\n");

Process *p2 = process_manager.create_process(64 * 1024, test_process_2).p;

if (!p2) {
  Debugger::log(Debugger::DebugType::PROCESS, "P2 CREATE FAILED\n");

  while (true)
    Kernel::halt();
}

Debugger::log(Debugger::DebugType::PROCESS, "P2 CREATED\n");

Process *p3 = process_manager.create_process(64 * 1024, test_process_3).p;

if (!p3) {
  Debugger::log(Debugger::DebugType::PROCESS, "P3 CREATE FAILED\n");

  while (true)
    Kernel::halt();
}

Debugger::log(Debugger::DebugType::PROCESS, "P3 CREATED\n");

Asm::sti();
*/
