#pragma once
#include "../core/kernel.hpp"
#include <stdarg.h>

namespace TerminalUtils {

  // This function clears the screen by iterating over every character and
  // swapping its value with ' ' the condition i < 80 * 25 * 2 represents that i
  // did not cover the whole screen the screen is 25 rows * 80 columns * 2 bytes
  // (character size)

  inline static void clear() {
    for (int i = 0; i < 80 * 25 * 2; i += 2) {
      Kernel::video_memory[i] = ' ';
      Kernel::video_memory[i + 1] = 0x0F;
    }
    Kernel::vram_index = 0;
  }

  // This function puts a character onto the screen
  // Each character is 2 bytes big:
  // 1 byte for data + 1 byte for color (eg: WHITE = 0x0F)
  inline static void putchar(char c) {
    if (c == '\n') {
      Kernel::vram_index = ((Kernel::vram_index + 160) / 160) * 160;
    } else if (c == '\b') {
      if (Kernel::vram_index > 0) {
        Kernel::vram_index -= 2;
        Kernel::video_memory[Kernel::vram_index] = ' ';
        Kernel::video_memory[Kernel::vram_index + 1] = 0x0F;
      }
    } else {
      Kernel::video_memory[Kernel::vram_index] = c;
      Kernel::video_memory[Kernel::vram_index + 1] = 0x0F;
      Kernel::vram_index += 2;
    }

    if (Kernel::vram_index >= 80 * 25 * 2) {
      Kernel::vram_index = 0; // Simple reset for now until scrolling is added
    }
  }

  // We use our putchar method to print characters to the screen
  inline static void print(const char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
      putchar(str[i]);
    }
  }

}; // namespace Terminal

