#include "../../data/system.hpp"

#pragma once

#include "../../utils/terminal_utils.hpp"
#include "icommand.hpp"

class WhoamiCommand : public ICommand {

public:
  const char *name() const { return "whoami"; }

  String execute(int argc, char **argv) {

    TerminalUtils::print(Generator::random_phrase(whoami_phrases));
    return "";
  }
};

