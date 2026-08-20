#pragma once

#include "../containers/vector.hpp"
#include "../shared/types.hpp"
#include "../utils/bit_utils.hpp"

static constexpr int PAGE_SIZE = 4096;
static constexpr u32 INVALID_PID = 0xFFFFFFFF;

struct AdressMap {
  u64 physical_addr;
  u64 virtual_addr;

  AdressMap(u64 v, u64 p) : physical_addr(p), virtual_addr(v) {};
  ~AdressMap() = default;
};

enum class FAllocatorEventType { ALLOCATED, FREE };

struct FrameAllocatorEvent {
  // physical address
  u64 physical_address;
  bool success;
  FAllocatorEventType type;

  FrameAllocatorEvent(u64 p, bool s, FAllocatorEventType t)
      : physical_address(p), success(s), type(t) {}
};

enum class ProcessState { RUNNING, READY, BLOCKED, TERMINATED };

enum class Priority { LOW, MEDIUM, HIGH };

struct ProcessId {
  u32 id;
  bool is_free;
};
