#pragma once

#include "cpu_context.hpp"
#include "page_table.hpp"
#include "ps_types.hpp"

static constexpr u64 PROCESS_MEMORY_START = 0x00400000;

static constexpr u64 STACK_BOTTOM = 0x00700000;
static constexpr size_t STACK_SIZE = 64 * 1024;
static constexpr size_t STACK_PAGES = (STACK_SIZE + PAGE_SIZE - 1) / PAGE_SIZE;
static constexpr u64 STACK_TOP = STACK_BOTTOM + STACK_PAGES * PAGE_SIZE;

class Process {
private:
  u32 pid = INVALID_PID;

  PageTable *page_table = nullptr;

  CpuContext ctx{};

  size_t size = 0;

  Priority priority = Priority::LOW;
  ProcessState state = ProcessState::BLOCKED;

  size_t allocated_pages = 0;
  size_t allocated_stack_pages = 0;

  bool is_init = false;

public:
  Process(PageTable *pt) : page_table(pt) {}

  Process(PageTable *pt, size_t size) : page_table(pt), size(size) {}

  ~Process() { clean_up(); }

  bool alloc() {
    if (!page_table)
      return false;

    u64 virtual_addr = STACK_BOTTOM;

    for (size_t i = 0; i < STACK_PAGES; i++) {
      FrameAllocatorEvent ev = page_table->get_frame_allocator().alloc();

      if (!ev.success) {
        clean_up_stack();
        return false;
      }

      bool ok = page_table->map(virtual_addr, ev.physical_address);

      if (!ok) {
        page_table->get_frame_allocator().free(ev.physical_address);

        clean_up_stack();
        return false;
      }

      allocated_stack_pages++;
      virtual_addr += PAGE_SIZE;
    }

    ctx.rsp = STACK_TOP;

    return true;
  }

  bool setup() {
    if (!size)
      return false;

    if (is_init)
      return false;

    if (!page_table)
      return false;

    const size_t pages = (size + PAGE_SIZE - 1) / PAGE_SIZE;

    if (!pages)
      return false;

    /*
     * Allocate normal process memory.
     */
    u64 virtual_addr = PROCESS_MEMORY_START;

    for (size_t i = 0; i < pages; i++) {
      FrameAllocatorEvent ev = page_table->alloc();

      if (!ev.success) {
        clean_up();
        return false;
      }

      bool ok = page_table->map(virtual_addr, ev.physical_address);

      if (!ok) {
        page_table->get_frame_allocator().free(ev.physical_address);

        clean_up();
        return false;
      }

      allocated_pages++;
      virtual_addr += PAGE_SIZE;
    }

    /*
     * Allocate the process stack.
     */
    if (!alloc()) {
      clean_up();
      return false;
    }

    state = ProcessState::READY;
    is_init = true;

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
