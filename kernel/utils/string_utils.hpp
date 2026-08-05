#pragma once

#include "../memory/memory.hpp"
#include <stdarg.h>

constexpr int DEFAULT_BUFFER_ALLOCATION_SIZE = 16;
constexpr int DEFAULT_BUFFER_INCREMENT = 16;

class StringUtils {

public:
  // this functions compares each char of 2 strings
  // if the letter is NOT the same, then they are different, therefore we return
  // a non-zero result if ALL the letters ARE the same, s1[i] - s2[i] will
  // return 0 since they are the same character

  inline static char scancode_to_ascii(unsigned char scancode) {
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

  inline static int strcmp(const char *s1, const char *s2) {
    int i = 0;
    while (s1[i] != '\0' && s2[i] != '\0') {
      if (s1[i] != s2[i]) {
        return s1[i] - s2[i];
      }
      i++;
    }
    return s1[i] - s2[i]; // Returns 0 if they match completely
  }

  inline static int split_by(char *str, const char separator, char **args,
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

  inline static int strlen(const char *str) {
    int i = 0;
    while (str[i] != '\0')
      i++;
    return i;
  }

  inline static const char *strcat(const char *str1, const char *str2) {

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

  inline static const char *iota(int val) {

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

  // Grows `buf` (currently `capacity` bytes, holding `used` valid bytes) so
  // it can hold at least `needed` bytes total (including the null
  // terminator). Returns the (possibly reallocated) buffer, or nullptr on
  // allocation failure. This is what was missing before: format() used to
  // kmalloc a fixed 16-byte buffer and write into it with no bounds check
  // at all, silently overflowing into the next heap block for any format
  // string longer than ~15 characters (e.g. the uptime string), corrupting
  // the allocator's free list and breaking unrelated allocations afterward.
  inline static char *grow_buffer(char *buf, size_t &capacity, int used,
                                  size_t needed) {
    if (needed <= capacity) {
      return buf;
    }

    size_t new_capacity = capacity;
    while (new_capacity < needed) {
      new_capacity += DEFAULT_BUFFER_INCREMENT;
    }

    char *new_buf = (char *)kmalloc(new_capacity);
    if (!new_buf) {
      return nullptr;
    }

    for (int i = 0; i < used; i++) {
      new_buf[i] = buf[i];
    }

    if (buf) {
      kfree(buf);
    }

    capacity = new_capacity;
    return new_buf;
  }

  inline static char *format(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    size_t capacity = DEFAULT_BUFFER_ALLOCATION_SIZE;
    char *buf = (char *)kmalloc(capacity);
    if (!buf) {
      va_end(args);
      return 0;
    }

    int buf_idx = 0;

    for (int i = 0; fmt[i] != '\0'; i++) {
      if (fmt[i] == '%' && fmt[i + 1] != '\0') {
        i++; // Skip the '%' and get the next character
        if (fmt[i] == 's') {
          const char *s = va_arg(args, const char *);
          while (*s) {
            buf = grow_buffer(buf, capacity, buf_idx, buf_idx + 2);
            if (!buf) {
              va_end(args);
              return 0;
            }
            buf[buf_idx++] = *s++;
          }
        } else if (fmt[i] == 'd') {
          int val = va_arg(args, int);
          const char *num_str = iota(val); // Convert int using iota
          while (*num_str) {
            buf = grow_buffer(buf, capacity, buf_idx, buf_idx + 2);
            if (!buf) {
              va_end(args);
              return 0;
            }
            buf[buf_idx++] = *num_str++;
          }
        } else if (fmt[i] == 'c') {
          char c =
              (char)va_arg(args, int); // Any type smaller than an int in ...
                                       // gets promoted to int by the compiler
          buf = grow_buffer(buf, capacity, buf_idx, buf_idx + 2);
          if (!buf) {
            va_end(args);
            return 0;
          }
          buf[buf_idx++] = c;
        }
      } else {
        buf = grow_buffer(buf, capacity, buf_idx, buf_idx + 2);
        if (!buf) {
          va_end(args);
          return 0;
        }
        buf[buf_idx++] = fmt[i];
      }
    }

    buf[buf_idx] = '\0'; // Null-terminate the final string
    va_end(args);

    return buf;
  }

  static void strcpy(char *dest, char *src) {
    if (!src || !dest)
      return;
    int i = 0;
    while (src[i] != '\0') {
      dest[i] = src[i];
      i++;
    }
    dest[i] = '\0';
  }

  static int count(char *str, char *c) {
    auto counter{0uz};
    auto i{0uz};
    while (str[i] != '\0') {
      if ((char)str[i] == c[0])
        counter++;
      i++;
    }
    return counter;
  }
};
