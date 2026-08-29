#pragma once

#include "../core/kernel.hpp"
#include "../core/timer.hpp"
#include "../storage/file_system.hpp"
#include "../tyrant_generator/generator.hpp"
#include "../utils/string_utils.hpp"
#include "../utils/terminal_utils.hpp"
#include "cli_apps/cli_app.hpp"
#include "cli_app_registry.hpp"
#include "commands/icommand.hpp"
#include "environment.hpp"
#include "terminal_registry.hpp"

class Terminal {
public:
  TerminalUtils &terminal_utils;
  FileSystem &fs;
  TerminalRegistry &terminal_reg;
  CliAppRegistry &cli_reg;
  Environment &env;

  /* Non-null while a CliApp is running. When this is set, parse()
     routes input to the app instead of doing normal command lookup.
     Only one app can be active at a time, apps are not nestable. */
  ICliApp *active_app = nullptr;

  Terminal(TerminalUtils &t_utils, FileSystem &fs, TerminalRegistry &reg,
          CliAppRegistry &cli_reg, Environment &env)
      : terminal_utils(t_utils), fs(fs), terminal_reg(reg), cli_reg(cli_reg),
        env(env) {}

  void draw_random_ascii() {
    terminal_utils.print(Generator::random_phrase(ascii_art),
                         Colors::pick_random_color());
  }

  void cd(char *path) {
    if (!path)
      return;
    u32 inode = fs.resolve_path(path);
    if (inode == INVALID_INODE) {
      terminal_utils.print("directory not found", 0xFFFFFF);
      return;
    }
    terminal_reg.current_dir = inode;
  }

  inline CommandResult parse(char *str, char **args, int max_args) {
    if (!str || !args || max_args <= 0)
      return CommandResult("\n");

    int argc = StringUtils::split_by(str, ' ', args, max_args);
    if (argc <= 0)
      return CommandResult("\n");

    /* While an app is active, every line goes to it first. This has
       to be checked before any normal command lookup, otherwise a
       running app could never see input that happens to match a
       real command name. */
    if (active_app != nullptr) {
      if (StringUtils::strcmp(args[0], active_app->quit_keyword()) == 0) {
        active_app->on_exit();
        active_app = nullptr;
        return CommandResult("\n");
      }

      bool keep_running = active_app->handle_input(argc, args);
      if (!keep_running) {
        active_app->on_exit();
        active_app = nullptr;
      }
      return CommandResult("\n");
    }

    ICommand *cmd = terminal_reg.find(args[0]);
    if (cmd != nullptr)
      return cmd->execute(argc, args);

    ICliApp *app = cli_reg.find(args[0]);
    if (app != nullptr) {
      active_app = app;
      active_app->on_enter();
      return CommandResult("\n");
    }

    return CommandResult(Generator::random_phrase(command_not_found_phrases),
                         Colors::RED);
  }

  Environment &get_env() { return env; }
  const char *get_current_path() { return fs.get_path(terminal_reg.current_dir); }
  void fill_registry() { 
    terminal_reg.fill_registry(); 
    cli_reg.fill_registry();
  }
};
