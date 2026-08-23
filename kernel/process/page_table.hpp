#pragma once
#include "../core/asm.hpp"
#include "../utils/debugger.hpp"
#include "frame_allocator.hpp"
#include "ps_types.hpp"

class PageTable {
private:
  static constexpr u64 ENTRY_PRESENT = 0x1;
  static constexpr u64 ENTRY_WRITABLE = 0x2;
  static constexpr u64 PAGE_MASK = ~0xFFFULL;
  static constexpr u64 ENTRY_USER = 0x4;

  FrameAllocator &frame_allocator;

  // Physical address of the root page table.
  u64 pml4;

  // Address decomposition

  u16 get_pml4_index(u64 virtual_addr) const {
    return (virtual_addr >> 39) & 0x1FF;
  }

  u16 get_pdpt_index(u64 virtual_addr) const {
    return (virtual_addr >> 30) & 0x1FF;
  }

  u16 get_pd_index(u64 virtual_addr) const {
    return (virtual_addr >> 21) & 0x1FF;
  }

  u16 get_pt_index(u64 virtual_addr) const {
    return (virtual_addr >> 12) & 0x1FF;
  }

  u64 get_page_offset(u64 virtual_addr) const { return virtual_addr & 0xFFF; }

  // Page-table helpers

  bool is_present(u64 entry) const { return entry & ENTRY_PRESENT; }

  u64 get_table_address(u64 entry) const { return entry & PAGE_MASK; }

  u64 *get_table(u64 physical_address) const {
    return reinterpret_cast<u64 *>(physical_address);
  }

  void clear_table(u64 physical_address) {
    u64 *table = get_table(physical_address);

    for (int i = 0; i < 512; i++)
      table[i] = 0;
  }

  bool table_is_empty(u64 physical_address) const {
    u64 *table = get_table(physical_address);

    for (int i = 0; i < 512; i++) {
      if (is_present(table[i]))
        return false;
    }

    return true;
  }

  // Allocate one page-table frame and initialize it.
  bool allocate_table(u64 &physical_address) {
    FrameAllocatorEvent ev = frame_allocator.alloc();

    if (!ev.success)
      return false;

    physical_address = ev.physical_address;

    u64 kernel_pml4 = Asm::read_cr3() & PAGE_MASK;

    if (physical_address == kernel_pml4) {
      Debugger::log("W ALLOCATED KERNEL PML4!\n");
    }

    clear_table(physical_address);

    return true;
  }
  // Get an existing child table or create one if it doesn't exist.
  bool get_or_create_table(u64 *parent_table, u16 index, u64 &child_address) {

    if (is_present(parent_table[index])) {
      child_address = get_table_address(parent_table[index]);
      return true;
    }

    if (!allocate_table(child_address))
      return false;

    parent_table[index] = child_address | ENTRY_PRESENT | ENTRY_WRITABLE;

    return true;
  }

public:
  PageTable(FrameAllocator &fallocator)
      : frame_allocator(fallocator), pml4(0) {}

  ~PageTable() = default;

  // Initialization

  bool init() {
    Debugger::log("PT: allocating PML4\n");

    if (!allocate_table(pml4)) {
      Debugger::log("PT: PML4 ALLOCATION FAILED\n");
      return false;
    }

    Debugger::log("PT: PML4 ALLOCATED\n");

    u64 kernel_pml4 = Asm::read_cr3() & PAGE_MASK;

    Debugger::log("PT: checking kernel PML4\n");

    if (!inherit_kernel_mappings(kernel_pml4)) {
      Debugger::log("PT: INHERIT FAILED\n");
      return false;
    }

    Debugger::log("PT: INIT SUCCESS\n");

    return true;
  }

  // Accessors

  FrameAllocator &get_frame_allocator() { return frame_allocator; }

  FrameAllocatorEvent alloc() { return frame_allocator.alloc(); }

  u64 get_pml4() const { return pml4; }

  // Mapping

  bool map(u64 virtual_addr, u64 physical_addr) {

    if (virtual_addr == 0 || physical_addr == 0)
      return false;

    if (virtual_addr % PAGE_SIZE != 0 || physical_addr % PAGE_SIZE != 0)
      return false;

    u64 *pml4_table = get_table(pml4);

    u64 pdpt_address;
    if (!get_or_create_table(pml4_table, get_pml4_index(virtual_addr),
                             pdpt_address))
      return false;

    u64 *pdpt_table = get_table(pdpt_address);

    u64 pd_address;
    if (!get_or_create_table(pdpt_table, get_pdpt_index(virtual_addr),
                             pd_address))
      return false;

    u64 *pd_table = get_table(pd_address);

    u16 pd_index = get_pd_index(virtual_addr);
    bool split_happened = false;

    if (is_present(pd_table[pd_index]) && is_huge(pd_table[pd_index])) {
      if (!split_huge_page(pd_table, pd_index))
        return false;
      split_happened = true;
    }

    u64 pt_address;
    if (!get_or_create_table(pd_table, pd_index, pt_address))
      return false;

    u64 *pt_table = get_table(pt_address);
    u16 pt_index = get_pt_index(virtual_addr);

    if (is_present(pt_table[pt_index]) && !split_happened) {
      // A genuine pre-existing mapping we didn't just create — refuse.
      return false;
    }
    // If split_happened, pt_table[pt_index] is leftover identity-map
    // filler from split_huge_page(), not a real allocation — safe to
    // overwrite with the caller's actual mapping.

    pt_table[pt_index] = physical_addr | ENTRY_PRESENT | ENTRY_WRITABLE;

    return true;
  }

  // Unmapping

  bool unmap(u64 virtual_addr) {

    if (virtual_addr == 0)
      return false;

    if (virtual_addr % PAGE_SIZE != 0)
      return false;

    u16 pml4_index = get_pml4_index(virtual_addr);
    u16 pdpt_index = get_pdpt_index(virtual_addr);
    u16 pd_index = get_pd_index(virtual_addr);
    u16 pt_index = get_pt_index(virtual_addr);

    u64 *pml4_table = get_table(pml4);

    // PML4 -> PDPT

    u64 pml4_entry = pml4_table[pml4_index];

    if (!is_present(pml4_entry))
      return false;

    u64 pdpt_address = get_table_address(pml4_entry);
    u64 *pdpt_table = get_table(pdpt_address);

    // PDPT -> PD

    u64 pdpt_entry = pdpt_table[pdpt_index];

    if (!is_present(pdpt_entry))
      return false;

    u64 pd_address = get_table_address(pdpt_entry);
    u64 *pd_table = get_table(pd_address);

    // PD -> PT

    u64 pd_entry = pd_table[pd_index];

    if (!is_present(pd_entry))
      return false;

    u64 pt_address = get_table_address(pd_entry);
    u64 *pt_table = get_table(pt_address);

    // PT -> Physical frame

    u64 &pt_entry = pt_table[pt_index];

    if (!is_present(pt_entry))
      return false;

    u64 physical_address = get_table_address(pt_entry);

    // Remove the actual mapping.
    pt_entry = 0;

    // Free the physical frame that was mapped.
    frame_allocator.free(physical_address);

    // PT cleanup

    if (table_is_empty(pt_address)) {

      frame_allocator.free(pt_address);

      pd_table[pd_index] = 0;

      // PD cleanup

      if (table_is_empty(pd_address)) {

        frame_allocator.free(pd_address);

        pdpt_table[pdpt_index] = 0;

        // PDPT cleanup

        if (table_is_empty(pdpt_address)) {

          frame_allocator.free(pdpt_address);

          pml4_table[pml4_index] = 0;
        }
      }
    }

    return true;
  }

  // Virtual -> Physical translation

  u64 find_physical_addr(u64 virtual_addr) const {

    u16 pml4_index = get_pml4_index(virtual_addr);
    u16 pdpt_index = get_pdpt_index(virtual_addr);
    u16 pd_index = get_pd_index(virtual_addr);
    u16 pt_index = get_pt_index(virtual_addr);

    u64 *pml4_table = get_table(pml4);

    // PML4
    u64 pml4_entry = pml4_table[pml4_index];

    if (!is_present(pml4_entry))
      return 0;

    // PDPT
    u64 pdpt_address = get_table_address(pml4_entry);
    u64 *pdpt_table = get_table(pdpt_address);

    u64 pdpt_entry = pdpt_table[pdpt_index];

    if (!is_present(pdpt_entry))
      return 0;

    // PD
    u64 pd_address = get_table_address(pdpt_entry);
    u64 *pd_table = get_table(pd_address);

    u64 pd_entry = pd_table[pd_index];

    if (!is_present(pd_entry))
      return 0;

    // PT
    u64 pt_address = get_table_address(pd_entry);
    u64 *pt_table = get_table(pt_address);

    u64 pt_entry = pt_table[pt_index];

    if (!is_present(pt_entry))
      return 0;

    // Physical frame + offset
    u64 physical_frame = get_table_address(pt_entry);

    return physical_frame + get_page_offset(virtual_addr);
  }

  bool inherit_kernel_mappings(u64 kernel_pml4) {
    if (!kernel_pml4)
      return false;

    u64 *new_pml4 = get_table(pml4);
    u64 *kernel_table = get_table(kernel_pml4);

    for (int i = 0; i < 512; i++) {
      if (is_present(kernel_table[i]))
        new_pml4[i] = kernel_table[i];
    }

    return true;
  }

  static void debug_kernel_pml4() {
    u64 kernel_pml4 = Asm::read_cr3() & PAGE_MASK;
    u64 *table = reinterpret_cast<u64 *>(kernel_pml4);

    for (int i = 0; i < 512; i++) {
      if (table[i] & ENTRY_PRESENT) {
        Debugger::logf("PML4[%d] PRESENT\n", i);
      }
    }
  }

  static constexpr u64 ENTRY_HUGE =
      0x80; // PS bit — marks a 2MiB page at the PD level

  bool is_huge(u64 entry) const { return entry & ENTRY_HUGE; }

  // If pd_table[pd_index] is currently a 2MiB huge page, replace it with a
  // freshly allocated page table that maps the exact same 2MiB physical
  // region at 4KiB granularity (same flags, minus the PS bit). This
  // preserves the existing identity mapping exactly — nothing that already
  // worked stops working — it just becomes overridable one 4KiB page at a
  // time from here on.
  bool split_huge_page(u64 *pd_table, u16 pd_index) {
    u64 entry = pd_table[pd_index];

    u64 huge_phys_base = entry & PAGE_MASK;    // 2MiB-aligned base
    u64 flags = (entry & 0xFFF) & ~ENTRY_HUGE; // keep present/writable, drop PS

    u64 new_pt_phys;
    if (!allocate_table(new_pt_phys))
      return false;

    u64 *new_pt = get_table(new_pt_phys);

    for (int i = 0; i < 512; i++)
      new_pt[i] = (huge_phys_base + (u64)i * PAGE_SIZE) | flags;

    pd_table[pd_index] = new_pt_phys | ENTRY_PRESENT | ENTRY_WRITABLE;

    return true;
  }
};
