#include "../../data/system.hpp"

#pragma once

#include "../../utils/terminal_utils.hpp"
#include "icommand.hpp"

class MotdCommand : public ICommand {

public:
  const char *name() const { return "motd"; }

  String execute(int argc, char **argv) {

    TerminalUtils::print(Generator::random_phrase(motd_messages));
    return "";
  }
};

