#pragma once

#include "cpu_context.hpp"
#include "page_table.hpp"
#include "ps_types.hpp"

static constexpr u64 PROCESS_MEMORY_START = 0x04000000; // 64 MiB
static constexpr u64 STACK_BOTTOM =
    0x08000000; // 128 MiB — separate region from process code, avoids collision
                // as sizes growstatic
constexpr size_t STACK_SIZE = 64 * 1024;
static constexpr size_t STACK_PAGES = (STACK_SIZE + PAGE_SIZE - 1) / PAGE_SIZE;
static constexpr u64 STACK_TOP = STACK_BOTTOM + STACK_PAGES * PAGE_SIZE;

using ProcessEntry = void (*)();

class Process {

private:
  u32 pid = INVALID_PID;

  PageTable *page_table = nullptr;

  CpuContext ctx{};

  size_t size = 0;
  ProcessEntry entry = nullptr;

  Priority priority = Priority::LOW;
  ProcessState state = ProcessState::BLOCKED;

  size_t allocated_pages = 0;
  size_t allocated_stack_pages = 0;

  bool is_init = false;

public:
  Process(PageTable *pt) : page_table(pt) {}

  Process(PageTable *pt, size_t size) : page_table(pt), size(size) {}

  Process(PageTable *pt, size_t size, ProcessEntry e)
      : page_table(pt), size(size), entry(e) {}

  ~Process() { clean_up(); }
  /*
  bool setup() {
    if (!size || is_init || !page_table || !entry)
      return false;

    const size_t pages = (size + PAGE_SIZE - 1) / PAGE_SIZE;
    if (!pages)
      return false;

    u64 virtual_addr = PROCESS_MEMORY_START;
    u8 *src = reinterpret_cast<u8 *>(entry);

    for (size_t i = 0; i < pages; i++) {
      FrameAllocatorEvent ev = page_table->alloc();
      if (!ev.success) {
        Debugger::logf("setup: frame alloc failed at page %d\n", (int)i);
        clean_up();
        return false;
      }

      page_table->unmap(virtual_addr);
      if (!page_table->map(virtual_addr, ev.physical_address)) {
        Debugger::logf("setup: map FAILED page=%d vaddr=%x phys=%x\n", (int)i,
                       (unsigned)virtual_addr, (unsigned)ev.physical_address);
        clean_up();
        return false;
      }

      u8 *dst = reinterpret_cast<u8 *>(ev.physical_address);
      size_t remaining = size - i * PAGE_SIZE;
      size_t copy_len = remaining < PAGE_SIZE ? remaining : PAGE_SIZE;

      for (size_t b = 0; b < copy_len; b++)
        dst[b] = src[i * PAGE_SIZE + b];

      allocated_pages++;
      virtual_addr += PAGE_SIZE;
    }

    Debugger::log("setup: code pages mapped OK, doing stack alloc()\n");

    if (!alloc()) {
      Debugger::log("setup: stack alloc() FAILED\n");
      clean_up();
      return false;
    }

    Debugger::log("setup: stack alloc OK, setup complete\n");

    state = ProcessState::READY;
    is_init = true;

    ctx.rip = PROCESS_MEMORY_START;
    ctx.cs = 0x08;
    ctx.ss = 0x10;
    ctx.rflags = 0x202;

    return true;
  }
*/

  bool setup() {
    if (!size || is_init || !page_table || !entry)
      return false;

    // TEMP: skip code copy entirely, just point at the function's
    // real address to isolate the scheduling test from the
    // code-relocation problem.
    ctx.rip = reinterpret_cast<u64>(entry);

    if (!alloc()) {
      clean_up();
      return false;
    }

    state = ProcessState::READY;
    is_init = true;

    ctx.cs = 0x08;
    ctx.ss = 0x10;
    ctx.rflags = 0x202;

    return true;
  }
  bool alloc() {
    if (!page_table)
      return false;

    u64 virtual_addr = STACK_BOTTOM;

    for (size_t i = 0; i < STACK_PAGES; i++) {
      FrameAllocatorEvent ev = page_table->get_frame_allocator().alloc();
      if (!ev.success) {
        Debugger::logf("alloc: stack frame alloc failed at page %d\n", (int)i);
        clean_up_stack();
        return false;
      }

      page_table->unmap(virtual_addr);

      if (!page_table->map(virtual_addr, ev.physical_address)) {
        Debugger::logf("alloc: stack map FAILED page=%d vaddr=%x phys=%x\n",
                       (int)i, (unsigned)virtual_addr,
                       (unsigned)ev.physical_address);
        clean_up_stack();
        return false;
      }

      allocated_stack_pages++;
      virtual_addr += PAGE_SIZE;
    }

    ctx.rsp = STACK_TOP;
    return true;
  }
  void terminate() { state = ProcessState::TERMINATED; }

  void block() { state = ProcessState::BLOCKED; }

  void unblock() {
    if (state != ProcessState::TERMINATED)
      state = ProcessState::READY;
  }

  void clean_up_stack() {
    if (!page_table)
      return;

    u64 virtual_addr = STACK_BOTTOM;

    for (size_t i = 0; i < allocated_stack_pages; i++) {
      page_table->unmap(virtual_addr);
      virtual_addr += PAGE_SIZE;
    }

    allocated_stack_pages = 0;
  }

  void clean_up() {
    if (page_table) {
      /*
       * Clean up normal process memory.
       */
      u64 virtual_addr = PROCESS_MEMORY_START;

      for (size_t i = 0; i < allocated_pages; i++) {
        page_table->unmap(virtual_addr);
        virtual_addr += PAGE_SIZE;
      }

      /*
       * Clean up stack.
       */
      clean_up_stack();

      delete page_table;
      page_table = nullptr;
    }

    pid = INVALID_PID;
    allocated_pages = 0;
    allocated_stack_pages = 0;
    is_init = false;
    state = ProcessState::TERMINATED;
  }

  void set_priority(Priority p) { priority = p; }

  bool is_ready() const { return state == ProcessState::READY; }

  bool is_running() const { return state == ProcessState::RUNNING; }

  void set_state(ProcessState s) { state = s; }

  void set_id(u32 val) { pid = val; }

  u32 get_pid() const { return pid; }

  void set_page_table(PageTable &pt) { page_table = &pt; }

  PageTable *get_page_table() { return page_table; }

  CpuContext &get_context() { return ctx; }
};
