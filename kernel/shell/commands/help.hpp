#pragma once

#include "icommand.hpp"
#include "../../utils/terminal_utils.hpp"

class HelpCommand : public ICommand {
public:
  const char *name() const { return "help"; }

//  const char *description() const { return "Displays every command."; }

  const char *execute(int argc, char **argv) {

    TerminalUtils::print("help      Display this menu\n");
    TerminalUtils::print("ls        List directory\n");
    TerminalUtils::print("cd        Change directory\n");
    TerminalUtils::print("pwd       Print working directory\n");
    TerminalUtils::print("touch     Create file\n");
    TerminalUtils::print("mkdir     Create directory\n");
    TerminalUtils::print("rm        Remove file\n");
    TerminalUtils::print("cat       Display file\n");
    TerminalUtils::print("echo      Print text\n");
    TerminalUtils::print("clear     Clear screen\n");

    return "";
  }
};
