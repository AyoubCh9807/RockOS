#include "../../data/system.hpp"

#pragma once

#include "../../utils/terminal_utils.hpp"
#include "icommand.hpp"

class WhoamiCommand : public ICommand {
private:
  TerminalUtils &terminal_utils;

public:
  WhoamiCommand(TerminalUtils &utils) : terminal_utils(utils) {};

  const char *name() const { return "whoami"; }

  CommandResult execute(int argc, char **argv) {

    return CommandResult(Generator::random_phrase(whoami_phrases), Colors::pick_random_color());
  }
};
