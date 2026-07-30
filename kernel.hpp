#pragma once

constexpr int KEYBOARD_BUFFER_SIZE = 1024;

constexpr int confirmation_port = 0x64;
constexpr int char_port = 0x60;
constexpr int release_hex = 0x80;
constexpr int special_key_hex = 0xE0;
constexpr int delete_hex = 0x53;
constexpr int backspace_hex = 0x0E;
constexpr int enter_hex = 0x1C;

typedef unsigned char u8;
typedef unsigned long long size_t;

inline unsigned char inb(unsigned short port) {
  unsigned char result;
  __asm__ volatile("inb %1, %0" : "=a"(result) : "Nd"(port));
  return result;
}

inline u8 *heap_start = (u8 *)0x00400000;
inline u8 *heap_end = (u8 *)0x00800000;
inline u8 *heap_current = (u8 *)0x00400000;

inline void *kmalloc(size_t size) {
  if (heap_current + size > heap_end) {
    return 0; // Out of memory
  }
  u8 *ptr = heap_current;
  heap_current += size;
  return (void *)ptr;
}

inline void *kfree(u8 *ptr) {
  // Nothin for now
  return 0;
}

inline char scancode_to_ascii(unsigned char scancode) {
  static const char ascii_map[128] = {
      0,   27,  '1',  '2',  '3',  '4', '5', '6',  '7', '8', '9', '0',
      '-', '=', '\b', '\t', 'a',  'z', 'e', 'r',  't', 'y', 'u', 'i',
      'o', 'p', '[',  ']',  '\n', 0,   'q', 's',  'd', 'f', 'g', 'h',
      'j', 'k', 'l',  'm',  '\'', '`', 0,   '\\', 'w', 'x', 'c', 'v',
      'b', 'n', ';',  ',',  '.',  '/', 0,   '*',  0,   ' '};

  if (scancode < 128) {
    return ascii_map[scancode];
  }
  return 0;
}

namespace Terminal {
  inline int vram_index = 0;
  inline volatile char *video_memory = (volatile char *)0xB8000;

  inline void clear() {
    for (int i = 0; i < 80 * 25 * 2; i += 2) {
      video_memory[i] = ' ';
      video_memory[i + 1] = 0x0F;
    }
    vram_index = 0;
  }

  inline void putchar(char c) {
    if (c == '\n') {
      vram_index = ((vram_index + 160) / 160) * 160;
    } else if (c == '\b') {
      if (vram_index > 0) {
        vram_index -= 2;
        video_memory[vram_index] = ' ';
        video_memory[vram_index + 1] = 0x0F;
      }
    } else {
      video_memory[vram_index] = c;
      video_memory[vram_index + 1] = 0x0F;
      vram_index += 2;
    }

    if (vram_index >= 80 * 25 * 2) {
      vram_index = 0; // Simple reset for now until scrolling is added
    }
  }

  inline void print(const char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
      putchar(str[i]);
    }
  }
}
