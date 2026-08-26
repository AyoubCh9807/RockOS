#pragma once

#include "../../data/system.hpp"

#include "../../utils/terminal_utils.hpp"
#include "icommand.hpp"

class LowerCommand : public ICommand {

private:
  TerminalUtils &terminal_utils;

public:
  LowerCommand(TerminalUtils &utils) : terminal_utils(utils) {}

  const char *name() const { return "lower"; }

  CommandResult execute(int argc, char **argv) {
    if (argc < 2 || !argv[1])
      return CommandResult(Generator::random_phrase(foolish_phrases));

    for(int i = 1; i < argc; i++) {
      StringUtils::lowercase(argv[i]);
      terminal_utils.print(Colors::WHITE, argv[i]);
      if(i < argc - 1) terminal_utils.print(Colors::WHITE, " ");
    } 

    return CommandResult("");
  }
};


