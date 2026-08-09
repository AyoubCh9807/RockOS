#pragma once

#include "../core/kernel.hpp"
#include "string_utils.hpp"

namespace TerminalUtils {

inline static void clear() {
  for (int i = 0; i < 80 * 25 * 2; i += 2) {
    Kernel::video_memory[i] = ' ';
    Kernel::video_memory[i + 1] = 0x0F;
  }
  Kernel::vram_cursor = 0;
  Kernel::vram_index = 0;
}

inline static void scroll() {
  for (int row = 1; row < 25; row++) {
    for (int col = 0; col < 80; col++) {
      int src = (row * 80 + col) * 2;
      int dst = ((row - 1) * 80 + col) * 2;

      Kernel::video_memory[dst] = Kernel::video_memory[src];
      Kernel::video_memory[dst + 1] = Kernel::video_memory[src + 1];
    }
  }

  for (int col = 0; col < 80; col++) {
    int index = (24 * 80 + col) * 2;

    Kernel::video_memory[index] = ' ';
    Kernel::video_memory[index + 1] = 0x0F;
  }
}
inline static void putchar(char c) {
  if (c == '\n') {
    // Move to the beginning of the next row.
    Kernel::vram_cursor = ((Kernel::vram_cursor / 80) + 1) * 80;
  }

  else if (c == '\b') {
    if (Kernel::vram_cursor > 0) {
      Kernel::vram_cursor--;

      Kernel::vram_index = Kernel::vram_cursor * 2;

      Kernel::video_memory[Kernel::vram_index] = ' ';
      Kernel::video_memory[Kernel::vram_index + 1] = 0x0F;
    }
  }

  else {
    Kernel::vram_index = Kernel::vram_cursor * 2;

    Kernel::video_memory[Kernel::vram_index] = c;
    Kernel::video_memory[Kernel::vram_index + 1] = 0x0F;

    Kernel::vram_cursor++;
  }

  if (Kernel::vram_cursor >= 80 * 25) {
    scroll();

    Kernel::vram_cursor = 24 * 80;
    Kernel::vram_index = Kernel::vram_cursor * 2;
  }
}

inline static void move_left() {
  if (Kernel::vram_cursor > 0)
    Kernel::vram_cursor--;

  Kernel::vram_index = Kernel::vram_cursor * 2;
}

inline static void move_right() {
  if (Kernel::vram_cursor < 80 * 25 - 1)
    Kernel::vram_cursor++;

  Kernel::vram_index = Kernel::vram_cursor * 2;
}

inline static void move_up() {
    if (Kernel::vram_cursor >= 80) {
        Kernel::vram_cursor -= 80;
        Kernel::vram_index = Kernel::vram_cursor * 2;
    }
}

inline static void move_down() {
    if (Kernel::vram_cursor + 80 < 80 * 25) {
        Kernel::vram_cursor += 80;
        Kernel::vram_index = Kernel::vram_cursor * 2;
    }
}

inline static void print(const char *str) {
  if (!str)
    return;
  for (int i = 0; str[i] != '\0'; i++) {
    putchar(str[i]);
  }
}

inline static void print_number(int n) {
  char buf[16];
  StringUtils::print_number_into(buf, sizeof(buf), n);
  print(buf);
}
} // namespace TerminalUtils
