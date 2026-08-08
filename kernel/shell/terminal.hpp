#pragma once

#include "../core/kernel.hpp"
#include "../core/timer.hpp"
#include "../storage/file_system.hpp"
#include "../tyrant_generator/generator.hpp"
#include "../utils/string_utils.hpp"
#include "../utils/terminal_utils.hpp"
#include "commands/icommand.hpp"
#include "terminal_registry.hpp"

class Terminal {
private:
  FileSystem &fs;
  TerminalRegistry &reg;

public:
  Terminal(FileSystem &fs, TerminalRegistry &reg) : fs(fs), reg(reg) {}

  void cd(char *path) {
    if (!path)
      return;
    u32 inode = fs.resolve_path(path);
    if (inode == INVALID_INODE) {
      TerminalUtils::print("directory not found");
      return;
    }
    reg.current_dir = inode;
  }

  inline const char *parse(char *str, char **args, int max_args) {
    if (!str || !args || max_args <= 0)
      return "\n";

    int argc = StringUtils::split_by(str, ' ', args, max_args);
    if (argc <= 0)
      return "\n";

    ICommand *cmd = reg.find(args[0]);
    if (cmd == nullptr)
      return "Command not found";

    return cmd->execute(argc, args);
  }

  const char *get_current_path() { return fs.get_path(reg.current_dir); }

  void fill_registry() { reg.fill_registry(); }
};
