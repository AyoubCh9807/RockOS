#pragma once

#include "../../utils/terminal_utils.hpp"
#include "icommand.hpp"

class HelpCommand : public ICommand {
public:
  const char *name() const { return "help"; }

  //  const char *description() const { return "Displays every command."; }

  String execute(int argc, char **argv) {
    return Generator::random_phrase(help_phrases);
  }
};
