#pragma once
#include "../shared/types.hpp"
#include "asm.hpp"  // <-- needed for Asm::inb / Asm::outb below; adjust path if asm.hpp lives elsewhere

struct idt_entry {
  u16 offset_low;
  u16 selector;
  u8 ist;
  u8 type_attr;
  u16 offset_middle;
  u32 offset_high;
  u32 reserved;
} __attribute__((packed));

struct idt_ptr {
  u16 limit;
  u64 base;
} __attribute__((packed));

inline idt_entry idt[256];
inline idt_ptr idt_p;

extern "C" void timer_stub();
extern "C" void default_stub();
extern "C" void keyboard_stub();
extern "C" void pagefault_stub();
extern "C" void gpfault_stub();
extern "C" void mouse_stub();
extern "C" void ata_stub();

inline void idt_set_gate(int n, u64 handler) {
  idt[n].offset_low = handler & 0xFFFF;
  // 0x08 = 64-bit kernel code segment
  idt[n].selector = 0x08;
  idt[n].ist = 0;
  idt[n].type_attr = 0x8E;
  idt[n].offset_middle = (handler >> 16) & 0xFFFF;
  idt[n].offset_high = (handler >> 32) & 0xFFFFFFFF;
  idt[n].reserved = 0;
}

inline void idt_init() {
  idt_p.limit = sizeof(idt_entry) * 256 - 1;
  idt_p.base = reinterpret_cast<u64>(&idt);

  // Give every vector the default handler first.
  for (int i = 0; i < 256; i++) {
    idt_set_gate(i, reinterpret_cast<u64>(default_stub));
  }

  // #GP -> vector 13
  idt_set_gate(13, reinterpret_cast<u64>(gpfault_stub));
  // #PF -> vector 14
  idt_set_gate(14, reinterpret_cast<u64>(pagefault_stub));

  // IRQ0 -> vector 32 -> PIT timer
  idt_set_gate(32, reinterpret_cast<u64>(timer_stub));
  // IRQ1 -> vector 33 -> keyboard
  idt_set_gate(33, reinterpret_cast<u64>(keyboard_stub));

  // IRQ12 -> vector 44 -> PS/2 mouse
  idt_set_gate(44, reinterpret_cast<u64>(mouse_stub));
  // IRQ14 -> vector 46 -> primary ATA channel (disk)
  idt_set_gate(46, reinterpret_cast<u64>(ata_stub));

  // Load the 64-bit IDT.
  asm volatile("lidt %0" : : "m"(idt_p) : "memory");

  // Unmask IRQ14 on the SLAVE PIC (port 0xA1) so the disk is actually
  // allowed to ring the bell. IRQ8-15 live on the slave PIC, and IRQ14
  // is the 7th line there (IRQ8=bit0 ... IRQ14=bit6), so we clear bit 6.
  //
  // NOTE: this assumes the PIC has already been remapped (vectors 32+)
  // by the time idt_init() runs, same as whatever already unmasked
  // IRQ0/IRQ1/IRQ12 for your timer/keyboard/mouse. If that remap/unmask
  // actually happens in a separate pic.hpp/pic_init() in your codebase,
  // move this two-line block there instead, right after the other IRQ
  // lines get unmasked - it just needs to run once, after the PIC remap
  // and before you expect disk interrupts to fire.
  u8 slave_mask = Asm::inb(0xA1);
  slave_mask &= ~(1 << 6);
  Asm::outb(0xA1, slave_mask);
}
