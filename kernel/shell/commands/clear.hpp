#pragma once

#include "../../utils/terminal_utils.hpp"
#include "icommand.hpp"

class ClearCommand : public ICommand {
private:
  TerminalUtils &terminal_utils;

public:
  ClearCommand(TerminalUtils &utils) : terminal_utils(utils) {};

  const char *name() const { return "clear"; }

  CommandResult execute(int argc, char **argv) {

    terminal_utils.clear();
    return CommandResult("", Colors::WHITE);
  }
};
