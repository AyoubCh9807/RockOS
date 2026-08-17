#pragma once

#include "../../utils/terminal_utils.hpp"
#include "icommand.hpp"

class HelpCommand : public ICommand {
public:
  const char *name() const { return "help"; }

  CommandResult execute(int argc, char **argv) {
    return CommandResult(Generator::random_phrase(help_phrases), Colors::pick_random_color());
  }
};
