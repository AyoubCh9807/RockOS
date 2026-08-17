#include "../../data/system.hpp"

#pragma once

#include "icommand.hpp"

class MotdCommand : public ICommand {

public:
  const char *name() const { return "motd"; }

  CommandResult execute(int argc, char **argv) {

    return CommandResult(Generator::random_phrase(motd_messages),
                         Colors::pick_random_color());
  }
};
