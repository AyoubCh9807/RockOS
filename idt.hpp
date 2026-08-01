#pragma once

struct idt_entry {
  unsigned short offset_lowerbits;
  unsigned short selector;
  unsigned char zero;
  unsigned char type_attr;
  unsigned short offset_higherbits;
} __attribute__((packed));

struct idt_ptr {
  unsigned short limit;
  unsigned int base;
} __attribute__((packed));

inline idt_entry idt[256];
inline idt_ptr idt_p;

extern "C" void timer_stub();
extern "C" void default_stub();
extern "C" void keyboard_stub();

inline void idt_set_gate(int n, unsigned int handler) {
  idt[n].offset_lowerbits = handler & 0xFFFF;
  idt[n].selector = 0x10;
  idt[n].zero = 0;
  idt[n].type_attr = 0x8E;
  idt[n].offset_higherbits = (handler >> 16) & 0xFFFF;
}

inline void idt_init() {
  idt_p.limit = sizeof(idt_entry) * 256 - 1;
  idt_p.base = (unsigned int)(unsigned long long)&idt;

  // Give every vector a safe fallback first, so anything we haven't
  // written a real handler for yet still returns cleanly instead of
  // hitting a not-present gate and triple-faulting.
  for (int i = 0; i < 256; i++) {
    idt_set_gate(i, (unsigned int)(unsigned long long)default_stub);
  }

  // Real handlers override the default above.
  idt_set_gate(32, (unsigned int)(unsigned long long)timer_stub);
  idt_set_gate(33, (unsigned int)(unsigned long long)keyboard_stub);
  __asm__ volatile("lidt (%0)" : : "r"(&idt_p));
}
