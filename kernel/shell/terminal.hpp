#pragma once
#include "../core/kernel.hpp"
#include "../core/timer.hpp"
#include <stdarg.h>
#pragma once

namespace Terminal {

// This function clears the screen by iterating over every character and
// swapping its value with ' ' the condition i < 80 * 25 * 2 represents that i
// did not cover the whole screen the screen is 25 rows * 80 columns * 2 bytes
// (character size)

inline void clear() {
  for (int i = 0; i < 80 * 25 * 2; i += 2) {
    Kernel::video_memory[i] = ' ';
    Kernel::video_memory[i + 1] = 0x0F;
  }
  Kernel::vram_index = 0;
}

// This function puts a character onto the screen
// Each character is 2 bytes big:
// 1 byte for data + 1 byte for color (eg: WHITE = 0x0F)
inline void putchar(char c) {
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
inline void print(const char *str) {
  for (int i = 0; str[i] != '\0'; i++) {
    putchar(str[i]);
  }
}

inline const char *parse(char *str, const char **args, int max_args) {
  int arg_count = StringUtils::split_by(str, ' ', args, max_args);

  if (arg_count <= 0)
    return "\n\0";

  if (StringUtils::strcmp(args[0], "help") == 0) {
    return "FULL GUIDE: \nHELP: help\nCODE: code\n\0";
  }
  if (StringUtils::strcmp(args[0], "version") == 0) {
    return "Rock OS version 1.0.0\n\0";
  }
  if (StringUtils::strcmp(args[0], "info") == 0) {
    return "OS: Rock OS V 1.0.0\n Creator: Ayoub Chemingui\n Shell: RockShell "
           "1.0.0\n WM: RockWM 1.0.0\n\0";
  }
  if (StringUtils::strcmp(args[0], "clear") == 0) {
    Terminal::clear();
    return "\n\0";
  } else {
    if (StringUtils::strcmp(args[0], "echo") == 0 && arg_count - 1 < max_args) {
      String res = "";
      for (int i = 1; i < arg_count; i++) {
        res = res + String(args[i]);
        if (i < arg_count - 1) {
          res = res + " ";
        }
      }
      res = res + "\n\0";
      return res.c_str(); 
    }
  }
  if (StringUtils::strcmp(args[0], "uptime") == 0) {
    return Timer::get_formatted_time();
  }
  if (StringUtils::strcmp(args[0], "reboot") == 0) {
    Kernel::reboot();
  }
  return "Unknown command\n\0";
}

} // namespace Terminal
