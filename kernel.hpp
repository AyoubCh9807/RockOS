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

// This function allocates memory
inline void *kmalloc(size_t size) {
  if (heap_current + size > heap_end) {
    return 0; // Out of memory
  }
  u8 *ptr = heap_current;
  heap_current += size;
  return (void *)ptr;
}

// This function frees memory
inline void *kfree(u8 *ptr) {
  // Nothin for now
  return 0;
}

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
inline int kstrcmp(const char *s1, const char *s2) {
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

inline int kstrlen(const char *str) {
  int i = 0;
  while (str[i] != '\0')
    i++;
  return i;
}

inline const char* kstrcat(const char *str1, const char *str2) {

  int len1 = kstrlen(str1);
  int len2 = kstrlen(str2);
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

inline const char *parse(char *str, const char **args, int max_args) {
  int arg_count = split_by(str, ' ', args, max_args);

  if (arg_count <= 0)
    return "\n\0";

  if (Terminal::kstrcmp(str, "help") == 0) {
    return "FULL GUIDE: \nHELP: help\nCODE: code\n\0";
  }
  if (Terminal::kstrcmp(str, "version") == 0) {
    return "Rock OS version 1.0.0\n\0";
  }
  if (Terminal::kstrcmp(str, "info") == 0) {
    return "OS: Rock OS V 1.0.0\n Creator: Ayoub Chemingui\n Shell: RockShell "
           "1.0.0\n WM: RockWM 1.0.0\n\0";
  }
  if (Terminal::kstrcmp(str, "clear") == 0) {
    Terminal::clear();
    return "\n\0";
  } else {
    if (kstrcmp(args[0], "echo") == 0 && arg_count == 2) {
      return kstrcat(args[1], "\n\0");
    }
  }
  return "Unkown command\n\0";
}

} // namespace Terminal
