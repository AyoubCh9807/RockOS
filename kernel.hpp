#pragma once
#include "memory.hpp"
#include "string.hpp"
#include "timer.hpp"

#include <stdarg.h>

#pragma once

constexpr int KEYBOARD_BUFFER_SIZE = 1024;
// Simple mapping from scancode to ascii character
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

// This function clears the screen by iterating over every character and
// swapping its value with ' ' the condition i < 80 * 25 * 2 represents that i
// did not cover the whole screen the screen is 25 rows * 80 columns * 2 bytes
// (character size)

inline void clear() {
  for (int i = 0; i < 80 * 25 * 2; i += 2) {
    video_memory[i] = ' ';
    video_memory[i + 1] = 0x0F;
  }
  vram_index = 0;
}

// This function puts a character onto the screen
// Each character is 2 bytes big:
// 1 byte for data + 1 byte for color (eg: WHITE = 0x0F)
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

// We use our putchar method to print characters to the screen
inline void print(const char *str) {
  for (int i = 0; str[i] != '\0'; i++) {
    putchar(str[i]);
  }
}

inline void reboot() {
  // Pulse the CPU reset line via the keyboard controller
  Asm::outb(keyboard_command_port, reboot_data);
}

inline const char *parse(char *str, const char **args, int max_args) {
  int arg_count = String::split_by(str, ' ', args, max_args);

  if (arg_count <= 0)
    return "\n\0";

  if (String::strcmp(str, "help") == 0) {
    return "FULL GUIDE: \nHELP: help\nCODE: code\n\0";
  }
  if (String::strcmp(str, "version") == 0) {
    return "Rock OS version 1.0.0\n\0";
  }
  if (String::strcmp(str, "info") == 0) {
    return "OS: Rock OS V 1.0.0\n Creator: Ayoub Chemingui\n Shell: RockShell "
           "1.0.0\n WM: RockWM 1.0.0\n\0";
  }
  if (String::strcmp(str, "clear") == 0) {
    Terminal::clear();
    return "\n\0";
  } else {
    if (String::strcmp(args[0], "echo") == 0 && arg_count == 2) {
      return String::strcat(args[1], "\n\0");
    }
  }
  if (String::strcmp(args[0], "uptime") == 0) {
    return Timer::get_formatted_time();
  }
  if (String::strcmp(args[0], "reboot") == 0) {
    reboot();
  }
  return "Unknown command\n\0";
}

} // namespace Terminal
