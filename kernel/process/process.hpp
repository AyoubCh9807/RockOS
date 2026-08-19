#pragma once

#include "../containers/vector.hpp"
#include "../shared/types.hpp"
#include "../utils/bit_utils.hpp"

static constexpr int PAGE_SIZE = 4096;
static constexpr u32 INVALID_PID = 0;

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

class FrameAllocator {
private:
  u8 *bitmap;
  u32 bitmap_size_bytes;
  u32 total_frames;

public:
  FrameAllocator(u32 total_memory) : bitmap(nullptr) {

    total_frames = total_memory / PAGE_SIZE;
    bitmap_size_bytes = (total_frames + 7) / 8;

    bitmap = new u8[bitmap_size_bytes];

    // Initially every frame is free.
    for (u32 i = 0; i < bitmap_size_bytes; i++)
      bitmap[i] = 0;
  }

  FrameAllocatorEvent alloc() {

    u32 bit = BitUtils::find_first_free_bit(bitmap, total_frames);

    if (bit == BitUtils::INVALID_BIT_INDEX) {
      return FrameAllocatorEvent(0, false, FAllocatorEventType::ALLOCATED);
    }

    BitUtils::set_bit(bitmap, bit);

    u64 physical_address = (u64)bit * PAGE_SIZE;

    return FrameAllocatorEvent(physical_address, true,
                               FAllocatorEventType::ALLOCATED);
  }

  FrameAllocatorEvent free(u64 physical_address) {

    if (physical_address == 0 || physical_address % PAGE_SIZE != 0) {

      return FrameAllocatorEvent(physical_address, false,
                                 FAllocatorEventType::FREE);
    }

    u32 frame = physical_address / PAGE_SIZE;

    if (frame >= total_frames) {
      return FrameAllocatorEvent(physical_address, false,
                                 FAllocatorEventType::FREE);
    }

    BitUtils::clear_bit(bitmap, frame);

    return FrameAllocatorEvent(physical_address, true,
                               FAllocatorEventType::FREE);
  }
};

class PageTable {
private:
  FrameAllocator &frame_allocator;
  u64 pml4;
  u64 pdpt;
  u64 pd;
  u64 pt;
  Vector<AdressMap> map_train;

public:
  PageTable(FrameAllocator &fallocator) : frame_allocator(fallocator) {}

  bool init() {
    FrameAllocatorEvent ev = frame_allocator.alloc();
    if (!ev.success)
      return false;

    pml4 = ev.physical_address;

    u64 *pml_table = reinterpret_cast<u64 *>(pml4);
    for (int i = 0; i < 512; i++) {
      pml_table[i] = 0;
    }

    return true;
  }

  FrameAllocator &get_frame_allocator() { return frame_allocator; }

  FrameAllocatorEvent alloc() { return frame_allocator.alloc(); }

  bool map(u64 virtual_addr, u64 physical_addr) {
    if (physical_addr == 0 || virtual_addr == 0)
      return false;

    u64 pml4_index = (virtual_addr >> 39) & 0x1FF;
    u64 pdpt_index = (virtual_addr >> 30) & 0x1FF;
    u64 pd_index = (virtual_addr >> 21) & 0x1FF;
    u64 pt_index = (virtual_addr >> 12) & 0x1FF;

    u64 *pml4_table = reinterpret_cast<u64 *>(pml4);

    // PML4 -> PDPT
    if (!(pml4_table[pml4_index] & 1)) {
      FrameAllocatorEvent ev = frame_allocator.alloc();

      if (!ev.success)
        return false;

      u64 pdpt_address = ev.physical_address;
      u64 *pdpt_table = reinterpret_cast<u64 *>(pdpt_address);

      for (int i = 0; i < 512; i++)
        pdpt_table[i] = 0;

      pml4_table[pml4_index] = pdpt_address | 0x3;
    }

    u64 pdpt_address = pml4_table[pml4_index] & ~0xFFFULL;
    u64 *pdpt_table = reinterpret_cast<u64 *>(pdpt_address);

    // PDPT -> PD
    if (!(pdpt_table[pdpt_index] & 1)) {
      FrameAllocatorEvent ev = frame_allocator.alloc();

      if (!ev.success)
        return false;

      u64 pd_address = ev.physical_address;
      u64 *pd_table = reinterpret_cast<u64 *>(pd_address);

      for (int i = 0; i < 512; i++)
        pd_table[i] = 0;

      pdpt_table[pdpt_index] = pd_address | 0x3;
    }

    u64 pd_address = pdpt_table[pdpt_index] & ~0xFFFULL;
    u64 *pd_table = reinterpret_cast<u64 *>(pd_address);

    // PD -> PT
    if (!(pd_table[pd_index] & 1)) {
      FrameAllocatorEvent ev = frame_allocator.alloc();

      if (!ev.success)
        return false;

      u64 pt_address = ev.physical_address;
      u64 *pt_table = reinterpret_cast<u64 *>(pt_address);

      for (int i = 0; i < 512; i++)
        pt_table[i] = 0;

      pd_table[pd_index] = pt_address | 0x3;
    }

    u64 pt_address = pd_table[pd_index] & ~0xFFFULL;
    u64 *pt_table = reinterpret_cast<u64 *>(pt_address);

    // PT -> physical frame
    pt_table[pt_index] = physical_addr | 0x3;

    map_train.push_back(AdressMap(virtual_addr, physical_addr));
  }

  void unmap(u64 virtual_addr) {
    u64 physical_addr = find_physical_addr(virtual_addr);
    for (int i = 0; i < map_train.size(); i++) {
      if (map_train[i].virtual_addr == virtual_addr) {
        frame_allocator.free(map_train[i].physical_addr);

        map_train.erase(i);
      }
    }
  }

  u64 find_physical_addr(u64 virtual_addr) {

    u64 pml4_index = (virtual_addr >> 39) & 0x1FF;
    u64 pdpt_index = (virtual_addr >> 30) & 0x1FF;
    u64 pd_index = (virtual_addr >> 21) & 0x1FF;
    u64 pt_index = (virtual_addr >> 12) & 0x1FF;
    u64 offset = virtual_addr & 0xFFF;

    u64 *pml4_table = reinterpret_cast<u64 *>(pml4);

    u64 pml4_entry = pml4_table[pml4_index];

    if (!(pml4_entry & 1))
      return 0;

    u64 pdpt_address = pml4_entry & ~0xFFFULL;

    u64 *pdpt_table = reinterpret_cast<u64 *>(pdpt_address);

    u64 pdpt_entry = pdpt_table[pdpt_index];

    if (!(pdpt_entry & 1))
      return 0;

    u64 pd_address = pdpt_entry & ~0xFFFULL;

    u64 *pd_table = reinterpret_cast<u64 *>(pd_address);

    u64 pd_entry = pd_table[pd_index];

    if (!(pd_entry & 1))
      return 0;

    u64 pt_address = pd_entry & ~0xFFFULL;

    u64 *pt_table = reinterpret_cast<u64 *>(pt_address);

    u64 pt_entry = pt_table[pt_index];

    if (!(pt_entry & 1))
      return 0;

    u64 physical_frame = pt_entry & ~0xFFFULL;

    return physical_frame + offset;
  }
};
enum class ProcessState { RUNNING, READY, BLOCKED, TERMINATED };

enum class Priority { LOW, MEDIUM, HIGH };

class Process {
private:
  u32 pid;

  PageTable *page_table;

  u64 stack_pointer;
  u64 heap_pointer;
  u64 instruction_pointer;

  size_t size;

  Priority priority;
  ProcessState state;

public:
  Process(PageTable *pt)
      : page_table(pt), stack_pointer(0), heap_pointer(0),
        instruction_pointer(0), size(0), priority(Priority::LOW),
        state(ProcessState::BLOCKED) {}
  Process(PageTable *pt, size_t size)
      : page_table(pt), stack_pointer(0), heap_pointer(0),
        instruction_pointer(0), size(size), priority(Priority::LOW),
        state(ProcessState::BLOCKED) {}
  ~Process();

  bool setup() {

    if (!size)
      return false;

    const u64 pages = (size + PAGE_SIZE - 1) / PAGE_SIZE;
    if (!pages)
      return false;

    if (!page_table)
      return false;

    u64 virtual_addr = 0x00400000;
    for (int i = 0; i < pages; i++) {
      // allocate a frame
      FrameAllocatorEvent ev = page_table->alloc();

      if (!ev.success) {
        clean_up();
        return false;
      };

      page_table->map(virtual_addr, ev.physical_address);

      virtual_addr += PAGE_SIZE;
    }

    state = ProcessState::READY;
    return true;
  };
  void terminate() { state = ProcessState::TERMINATED; }

  void block() { state = ProcessState::BLOCKED; };
  void unblock() {
    if (state != ProcessState::TERMINATED)
      state = ProcessState::READY;
  };

  void clean_up() {
    u64 virtual_addr = 0x00400000;
    const u64 pages = (size + PAGE_SIZE - 1) / PAGE_SIZE;

    for (int i = 0; i < pages; i++) {
      page_table->unmap(virtual_addr);
      virtual_addr += PAGE_SIZE;
    }

    delete page_table;
    page_table = nullptr;
  }

  void set_priority(Priority p) { priority = p; };

  bool is_ready() const { return state == ProcessState::READY; };
  bool is_running() const { return state == ProcessState::RUNNING; };

  void set_id(u32 val) { pid = val; }

  u32 get_pid() { return pid; }

  void set_page_table(PageTable &pt) {
    if (pt != nullptr)
      page_table = &pt;
  }
};

struct ProcessId {
  u32 id;
  bool is_free;
};

class ProcessManager {
private:
  static constexpr int MAX_PROCESSES = 1024;
  Vector<Process> process_train;

  FrameAllocator &frame_allocator;

  ProcessId id_pool[MAX_PROCESSES];

public:
  ProcessManager(FrameAllocator &falloc) : frame_allocator(falloc) {
    for(int i = 0; i < MAX_PROCESSES; i++) {
      ProcessId pid;
      pid.id = i;
      pid.is_free = true;
      id_pool[i] = pid;
    }

  }

  bool create_process(size_t size) {

    PageTable *pt = new PageTable(frame_allocator);

    if (!pt->init()) {
      delete pt;
      return false;
    }

    Process p(pt, size);

    u32 pid = get_free_id();

    if (pid == INVALID_PID) {
      delete pt;
      return false;
    }

    p.set_id(pid);

    if (!p.setup()) {
      delete pt;
      return false;
    }

    process_train.push_back(p);

    id_pool[pid].is_free = false;

    return true;
  }

  u32 get_free_id() {
    for (int i = 0; i < MAX_PROCESSES; i++) {
      if (id_pool[i].is_free)
        return id_pool[i].id;
    }
    return INVALID_PID;
  }

  bool delete_process(u32 pid) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
      if (process_train[i].get_pid() == pid) {

        process_train[i].terminate();
        process_train[i].clean_up();
        process_train.erase(i);
      }
    }
  }
};
