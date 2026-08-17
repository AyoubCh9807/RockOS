#pragma once

#include "../core/kernel.hpp"
#include "../core/timer.hpp"
#include "../storage/file_system.hpp"
#include "../tyrant_generator/generator.hpp"
#include "../utils/string_utils.hpp"
#include "../utils/terminal_utils.hpp"
#include "commands/icommand.hpp"
#include "environment.hpp"
#include "terminal_registry.hpp"

class Terminal {
public:
  // private:

  TerminalUtils& terminal_utils;
  FileSystem &fs;
  TerminalRegistry &reg;
  Environment &env;

  // public:
  Terminal(TerminalUtils& t_utils, FileSystem &fs, TerminalRegistry &reg, Environment &env)
      : fs(fs), reg(reg), env(env), terminal_utils(t_utils) {}

  void draw_random_ascii() {
    terminal_utils.print(Generator::random_phrase(ascii_art), 0xFFFFFF);
  }

  void cd(char *path) {
    if (!path)
      return;
    u32 inode = fs.resolve_path(path);
    if (inode == INVALID_INODE) {
      terminal_utils.print("directory not found", 0xFFFFFF);
      return;
    }
    reg.current_dir = inode;
  }

  inline CommandResult parse(char *str, char **args, int max_args) {
    if (!str || !args || max_args <= 0)
      return CommandResult("\n");

    int argc = StringUtils::split_by(str, ' ', args, max_args);
    if (argc <= 0)
      return CommandResult("\n");

    ICommand *cmd = reg.find(args[0]);
    if (cmd == nullptr)
      return CommandResult(Generator::random_phrase(command_not_found_phrases), Colors::RED);

    return cmd->execute(argc, args);
  }

  Environment &get_env() { return env; }

  const char *get_current_path() { return fs.get_path(reg.current_dir); }

  void fill_registry() { reg.fill_registry(); }
};
