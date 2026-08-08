#pragma once

#include "../utils/string_utils.hpp"
#include "commands/cat.hpp"
#include "commands/cd.hpp"
#include "commands/clear.hpp"
#include "commands/damian.hpp"
#include "commands/echo.hpp"
#include "commands/help.hpp"
#include "commands/icommand.hpp"
#include "commands/ls.hpp"
#include "commands/mkdir.hpp"
#include "commands/pwd.hpp"
#include "commands/reboot.hpp"
#include "commands/rm.hpp"
#include "commands/rmdir.hpp"
#include "commands/touch.hpp"
#include "commands/tyrant.hpp"
#include "commands/uptime.hpp"

constexpr int MAX_TERMINAL_COMMANDS = 256;

class TerminalRegistry {
private:
  ICommand *commands[MAX_TERMINAL_COMMANDS]{};

  RebootCommand reboot;
  ClearCommand clear;
  EchoCommand echo;
  UptimeCommand uptime;
  HelpCommand help;
  LsCommand ls;
  MkdirCommand mkdir;
  PwdCommand pwd;
  TyrantCommand tyrant;
  DamianCommand damian;
  CdCommand cd;
  TouchCommand touch;
  RmCommand rm;
  RmdirCommand rmdir;
  CatCommand cat;

  int count = 0;

public:
  u32 &current_dir;

  TerminalRegistry(FileSystem &fs, u32 &current_dir)
      : current_dir(current_dir), reboot(), clear(), echo(), uptime(), help(),
        ls(fs, current_dir), mkdir(fs, current_dir), pwd(fs, current_dir),
        tyrant(), damian(), cd(fs, current_dir), touch(fs, current_dir),
        rm(fs, current_dir), rmdir(fs, current_dir), cat(fs, current_dir) {}

  void register_command(ICommand *cmd) { commands[count++] = cmd; }

  void fill_registry() {
    register_command(&reboot);
    register_command(&clear);
    register_command(&echo);
    register_command(&uptime);
    register_command(&help);
    register_command(&ls);
    register_command(&mkdir);
    register_command(&pwd);
    register_command(&tyrant);
    register_command(&damian);
    register_command(&cd);
    register_command(&touch);
    register_command(&rm);
    register_command(&rmdir);
    register_command(&cat);

    TerminalUtils::print(StringUtils::format("Loaded %d commands successfully!", count));
  }

  ICommand *find(char *name) {
    for (int i = 0; i < count; i++) {
      if (StringUtils::strcmp(name, commands[i]->name()) == 0)
        return commands[i];
    }
    return nullptr;
  }
};
