#pragma once

#include "../../data/system.hpp"

#include "../../utils/terminal_utils.hpp"
#include "icommand.hpp"

class RepeatCommand : public ICommand {

private:
  TerminalUtils &terminal_utils;

public:
  RepeatCommand(TerminalUtils &utils) : terminal_utils(utils) {}

  const char *name() const { return "repeat"; }

  CommandResult execute(int argc, char **argv) {
    if (argc < 3 || !argv[1] || !argv[2])
      return CommandResult(Generator::random_phrase(foolish_phrases));

    int repeat_times = StringUtils::to_int(argv[1]);
    if (repeat_times <= 0)
      return CommandResult("");

    for (int i = 0; i < repeat_times; i++) {
      for (int j = 2; j < argc; j++) {
        terminal_utils.print(argv[j], Colors::WHITE);

        if (j < argc - 1)
          terminal_utils.print(" ", Colors::WHITE);
      }
      terminal_utils.print("\n", Colors::WHITE);
    }

    return CommandResult("");
  }
};
