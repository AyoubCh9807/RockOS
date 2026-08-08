#pragma once

#include "../../utils/terminal_utils.hpp"
#include "icommand.hpp"

class ClearCommand : public ICommand {

public:
  const char *name() const { return "clear"; }

  const char *description() const { return "Clear terminal."; }

  String execute(int argc, char **argv) {

    TerminalUtils::clear();
    return "";
  }
};
