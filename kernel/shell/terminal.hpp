#pragma once
#include "../core/kernel.hpp"
#include "../core/timer.hpp"
#include "../storage/file_system.hpp"
#include "../tyrant_generator/generator.hpp"
#include <stdarg.h>

class Terminal {

private:
  FileSystem &fs;
  u32 current_dir;

public:
  Terminal(FileSystem &fs) : fs(fs), current_dir(ROOT_INODE) {}
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

  void cd(char *path) {
    u32 inode = fs.resolve_path(path);

    if (inode == INVALID_INODE) {
      print("directory not found");
      return;
    }

    current_dir = inode;
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

  inline const char *parse(char *str, char **args, int max_args) {
    int arg_count = StringUtils::split_by(str, ' ', args, max_args);

    if (arg_count <= 0)
      return "\n\0";

    if (StringUtils::strcmp(args[0], "help") == 0) {
      return "=== ROCK OS COMMAND CENTER ===\n"
             "  help    - Displays this command guide\n"
             "  info    - System specs & creator details\n"
             "  version - Current kernel & environment build\n"
             "  echo    - Echoes text back to the terminal\n"
             "  uptime  - Shows active system runtime\n"
             "  clear   - Wipes the display buffer\n"
             "  reboot  - Triggers a hardware reset\n\0";
    }
    if (StringUtils::strcmp(args[0], "version") == 0) {
      return "Rock OS [Version 1.0.0-alpha]\n"
             "(c) 2026 Ayoub Chemingui. All rights reserved.\n"
             "Target: Independent Custom Kernel\n\0";
    }
    if (StringUtils::strcmp(args[0], "info") == 0) {
      return "========================================\n"
             "  OS Architecture: Rock OS (x86 Bare-Metal)\n"
             "  Kernel Version:  1.0.0-release\n"
             "  Lead Developer:  Ayoub Chemingui\n"
             "  Environment:     RockShell 1.0.0 / RockWM\n"
             "  Video Buffer:    0xB8000 (Text Mode 80x25)\n"
             "========================================\n\0";
    }
    if (StringUtils::strcmp(args[0], "clear") == 0) {
      Terminal::clear();
      return "\n\0";
    } else {
      if (StringUtils::strcmp(args[0], "echo") == 0 &&
          arg_count - 1 < max_args) {
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
    if (StringUtils::strcmp(args[0], "tyrant") == 0 ||
        StringUtils::strcmp(args[0], "damian") == 0) {

      return Generator::generate_words();
    }
    if (StringUtils::strcmp(args[0], "ls") == 0) {
      return fs.list_directory(current_dir);
    }

    if (StringUtils::strcmp(args[0], "touch") == 0) {

      if (arg_count < 2)
        return "usage: touch <filename>\n";

      const char *current_path = fs.get_path(current_dir);

      char *file_path = StringUtils::format("%s/%s", current_path, args[1]);

      fs.create(file_path, false);
    }

    return "Command not recognized. Type 'help' for a list of valid "
           "commands.\n\0";
  }

}; // namespace Terminal
