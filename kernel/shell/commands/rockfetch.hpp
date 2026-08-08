#include "../../data/system.hpp"

#pragma once

#include "../../utils/terminal_utils.hpp"
#include "icommand.hpp"

class RockfetchCommand : public ICommand {

public:
  const char *name() const { return "rockfetch"; }

  String execute(int argc, char **argv) {
    for(int i = 0; i < DIAGNOSTIC_FIELD_COUNT; i++) {
      TerminalUtils::print(StringUtils::format("%s: %s\n", diagnostic_labels[i], diagnostic_values[i]));
    }
    return "";

  }
};

