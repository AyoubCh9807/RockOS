#pragma once
#include "../shared/types.hpp"

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

  // Load the 64-bit IDT.
  asm volatile(
      "lidt %0"
      :
      : "m"(idt_p)
      : "memory");
}
