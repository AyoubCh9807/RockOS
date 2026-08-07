#pragma once
// #include "../commands/icommand.hpp"
#include "../core/kernel.hpp"
#include "../core/timer.hpp"
#include "../storage/file_system.hpp"
#include "../tyrant_generator/generator.hpp"
#include "../utils/terminal_utils.hpp"
#include "terminal_registry.hpp"

#include <stdarg.h>

class Terminal {

private:
  FileSystem &fs;
  u32 current_dir;
  TerminalRegistry& reg;

public:
  Terminal(FileSystem &fs, TerminalRegistry &reg)
      : fs(fs), reg(reg), current_dir(ROOT_INODE) {
        reg.fill_registry();
        
      }
  // This function clears the screen by iterating over every character and
  // swapping its value with ' ' the condition i < 80 * 25 * 2 represents that i
  // did not cover the whole screen the screen is 25 rows * 80 columns * 2 bytes
  // (character size)

  void cd(char *path) {
    u32 inode = fs.resolve_path(path);

    if (inode == INVALID_INODE) {
      TerminalUtils::print("directory not found");
      return;
    }

    current_dir = inode;
  }
  inline const char *parse(char *str, char **args, int max_args) {
    int argc = StringUtils::split_by(str, ' ', args, max_args);

    if (argc <= 0)
      return "\n\0";

    ICommand *cmd = reg.find(args[0]);
    if (cmd == nullptr)
      return "Command not found";
    // Too lazy to rename args to argv
    return cmd->execute(argc, args);
  }

}; // namespace Terminal
