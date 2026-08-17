#pragma once
#include "../../data/system.hpp"

#include "../../utils/terminal_utils.hpp"
#include "icommand.hpp"

class DiagnoseCommand : public ICommand {

private:
  TerminalUtils &terminal_utils;

public:
  DiagnoseCommand(TerminalUtils &utils) : terminal_utils(utils) {}

  const char *name() const { return "diagnose"; }

  CommandResult execute(int argc, char **argv) {

    for (int i = 0; i < DIAGNOSTIC_FIELD_COUNT; i++) {
      terminal_utils.print(StringUtils::format("%s: %s\n", diagnostic_labels[i],
                                               diagnostic_values[i]), 0xFFFFFF);
    }

    return CommandResult("");
  }
};
