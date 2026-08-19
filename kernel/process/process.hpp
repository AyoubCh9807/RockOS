#pragma once

#include "../shared/types.hpp"

struct PageTable;

enum class ProcessState { RUNNING, READY, BLOCKED, TERMINATED };

enum class Priority { LOW, MEDIUM, HIGH };

struct Process {

  u32 pid;

  // Root of this process's virtual address space.
  PageTable *page_table;

  // Virtual stack pointer (RSP).
  u64 stack_pointer;

  // Virtual address of the next instruction (RIP).
  u64 instruction_pointer;

  Priority priority;

  ProcessState state;
};
