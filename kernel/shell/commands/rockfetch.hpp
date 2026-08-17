#include "../../data/system.hpp"

#pragma once

#include "../../utils/terminal_utils.hpp"
#include "icommand.hpp"

class RockfetchCommand : public ICommand {

private:
  TerminalUtils &terminal_utils;

public:
  RockfetchCommand(TerminalUtils &utils) : terminal_utils(utils) {}

  const char *name() const { return "rockfetch"; }

  CommandResult execute(int argc, char **argv) {
    for (int i = 0; i < DIAGNOSTIC_FIELD_COUNT; i++) {
      terminal_utils.print(StringUtils::format("%s: %s\n", diagnostic_labels[i],
                                               diagnostic_values[i]), Colors::RED);
    }
  }
};
