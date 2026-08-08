#pragma once
#include "../../data/system.hpp"

#include "../../utils/terminal_utils.hpp"
#include "icommand.hpp"

class StatsCommand : public ICommand {

public:
  const char *name() const { return "stats"; }

  String execute(int argc, char **argv) {

    for (int i = 0; i < STAT_FIELD_COUNT; i++) {
      TerminalUtils::print(
          StringUtils::format("%s: %s\n", stat_labels[i], stat_values[i]));
    }

    return "";
  }
};
