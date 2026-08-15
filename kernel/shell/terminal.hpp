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
private:
  FileSystem &fs;
  TerminalRegistry &reg;
  Environment& env;

public:
  Terminal(FileSystem &fs, TerminalRegistry &reg, Environment& env) : fs(fs), reg(reg), env(env) {}


  void draw_random_ascii() {
    TerminalUtils::print(Generator::random_phrase(ascii_art));
  }

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

  inline String parse(char *str, char **args, int max_args) {
    if (!str || !args || max_args <= 0)
      return "\n";

    int argc = StringUtils::split_by(str, ' ', args, max_args);
    if (argc <= 0)
      return "\n";

    ICommand *cmd = reg.find(args[0]);
    if (cmd == nullptr)
      return Generator::random_phrase(command_not_found_phrases); 

    return cmd->execute(argc, args);
  }

  Environment& get_env() { return env; }

  const char *get_current_path() { return fs.get_path(reg.current_dir); }

  void fill_registry() { reg.fill_registry(); }
};
