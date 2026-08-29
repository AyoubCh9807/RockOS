#pragma once

#include "../../data/system.hpp"

#include "../../utils/terminal_utils.hpp"
#include "icommand.hpp"

class LengthCommand : public ICommand {

private:
  TerminalUtils &terminal_utils;

public:
  LengthCommand(TerminalUtils &utils) : terminal_utils(utils) {}

  const char *name() const { return "length"; }

  CommandResult execute(int argc, char **argv) {
    if (argc < 2 || !argv[1])
      return CommandResult(Generator::random_phrase(foolish_phrases));

    int total = 0;

    for(int i = 1; i < argc; i++) {
      if(i < argc - 1) total += 1;
      total += StringUtils::strlen(argv[i]);
    } 

    char buf[256];

    StringUtils::snprintf(buf, sizeof(buf), "Damian calculated the length himself and found it to be %d", total);
    terminal_utils.print(buf, Colors::WHITE);

    return CommandResult("");
  }
};



