#pragma once
#include "timer.hpp"
#include "string.hpp"
#include "memory.hpp"

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

// this functions compares each char of 2 strings
// if the letter is NOT the same, then they are different, therefore we return a
// non-zero result if ALL the letters ARE the same, s1[i] - s2[i] will return 0
// since they are the same character

/*
 *clear
TODO: echo
help
info
TODO: reboot
TODO: uptime
version
 * */


inline const char* parse(char *str, const char **args, int max_args) {
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
  if(String::strcmp(args[0], "uptime") == 0) {
    return Timer::get_formatted_time();
  }
  return "Unknown command\n\0";
}

inline const char *kiota(int val) {

  if (val == 0)
    return "0\0";

  char map[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

  int val_length = 0;
  int temp_val = val;

  while (temp_val != 0) {
    temp_val /= 10;
    val_length++;
  }

  char *str = (char *)kmalloc(val_length + 1);
  if (!str)
    return "\0";

  temp_val = val;

  for (int i = val_length - 1; i >= 0; i--) {
    // 150 -> 0 then 5 then 1
    str[i] = map[temp_val % 10];
    temp_val /= 10;
  }

  str[val_length] = '\0';

  return str;
}

inline char* kformat(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    // Allocate a safe buffer on the heap for our formatted string
    char* buf = (char*)kmalloc(DEFAULT_BUFFER_ALLOCATION_SIZE);
    if (!buf) return 0;

    int buf_idx = 0;

    for (int i = 0; fmt[i] != '\0'; i++) {
        if (fmt[i] == '%' && fmt[i + 1] != '\0') {
            i++; // Skip the '%' and get the next character
            if (fmt[i] == 's') {
                const char* s = va_arg(args, const char*);
                while (*s) {
                    buf[buf_idx++] = *s++;
                }
            } else if (fmt[i] == 'd') {
                int val = va_arg(args, int);
                const char* num_str = kiota(val); // Convert int using kiota
                while (*num_str) {
                    buf[buf_idx++] = *num_str++;
                }
            } else if (fmt[i] == 'c') {
                char c = (char)va_arg(args, int); // Any type smaller than an int in ... gets promoted to int by the compiler
                buf[buf_idx++] = c;
            }
        } else {
            buf[buf_idx++] = fmt[i];
        }
    }

    buf[buf_idx] = '\0'; // Null-terminate the final string
    va_end(args);

    return buf;
}

} // namespace Terminal
