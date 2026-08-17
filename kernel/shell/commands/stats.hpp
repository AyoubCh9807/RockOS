#pragma once
#include "../../data/system.hpp"

#include "../../utils/terminal_utils.hpp"
#include "icommand.hpp"

class StatsCommand : public ICommand {

private:
  TerminalUtils terminal_utils;

public:
  StatsCommand(TerminalUtils &utils) : terminal_utils(utils) {}

  const char *name() const { return "stats"; }

  CommandResult execute(int argc, char **argv) {

    for (int i = 0; i < STAT_FIELD_COUNT; i++) {
      terminal_utils.print(
          StringUtils::format("%s: %s\n", stat_labels[i], stat_values[i]),
          Colors::GREEN);
    }

  }
};
