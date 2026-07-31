#pragma once

#include <stdarg.h>
#include "memory.hpp"

constexpr int DEFAULT_BUFFER_ALLOCATION_SIZE = 256;

namespace String {

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

// this functions compares each char of 2 strings
// if the letter is NOT the same, then they are different, therefore we return a
// non-zero result if ALL the letters ARE the same, s1[i] - s2[i] will return 0
// since they are the same character
inline int strcmp(const char *s1, const char *s2) {
  int i = 0;
  while (s1[i] != '\0' && s2[i] != '\0') {
    if (s1[i] != s2[i]) {
      return s1[i] - s2[i];
    }
    i++;
  }
  return s1[i] - s2[i]; // Returns 0 if they match completely
}

inline int split_by(char *str, const char separator, const char **args,
                    int max_args) {
  int arg_count = 0;
  bool in_word = false;

  for (int i = 0; str[i] != '\0'; i++) {
    if (str[i] == separator) {
      str[i] = '\0'; // Cut off the string right at the comma
      in_word = false;
    } else if (str[i] != ' ' && str[i] != '\n' && str[i] != '\r') {
      if (!in_word) {
        if (arg_count < max_args) {
          args[arg_count] = &str[i]; // Point to the start of the next piece
          arg_count++;
        }
        in_word = true;
      }
    }
  }
  return arg_count;
}

/*
 *clear
TODO: echo
help
info
TODO: reboot
TODO: uptime
version
 * */

inline int strlen(const char *str) {
  int i = 0;
  while (str[i] != '\0')
    i++;
  return i;
}

inline const char *strcat(const char *str1, const char *str2) {

  int len1 = strlen(str1);
  int len2 = strlen(str2);
  char *dest = (char *)kmalloc(len1 + len2 + 1);

  if (!dest) {
    return "Out of memory\n\0";
  }

  int current_index = 0;

  for (int i = 0; i < len1; i++) {
    dest[current_index++] = str1[i];
  }

  for (int i = 0; i < len2; i++) {
    dest[current_index++] = str2[i];
  }

  dest[current_index] = '\0';

  return dest;
}

inline const char *iota(int val) {

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

inline char *format(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);

  // Allocate a safe buffer on the heap for our formatted string
  char *buf = (char *)kmalloc(DEFAULT_BUFFER_ALLOCATION_SIZE);
  if (!buf)
    return "";

  int buf_idx = 0;

  for (int i = 0; fmt[i] != '\0'; i++) {
    if (fmt[i] == '%' && fmt[i + 1] != '\0') {
      i++; // Skip the '%' and get the next character
      if (fmt[i] == 's') {
        const char *s = va_arg(args, const char *);
        while (*s) {
          buf[buf_idx++] = *s++;
        }
      } else if (fmt[i] == 'd') {
        int val = va_arg(args, int);
        const char *num_str = iota(val); // Convert int using iota
        while (*num_str) {
          buf[buf_idx++] = *num_str++;
        }
      } else if (fmt[i] == 'c') {
        char c =
            (char)va_arg(args, int); // Any type smaller than an int in ... gets
                                     // promoted to int by the compiler
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

} // namespace String
