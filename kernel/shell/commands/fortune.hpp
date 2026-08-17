#pragma once
#include "../../data/fortune.hpp"

#pragma once

#include "../../utils/terminal_utils.hpp"
#include "icommand.hpp"

class FortuneCommand : public ICommand {

public:
  const char *name() const { return "fortune"; }

  CommandResult execute(int argc, char **argv) {

    return CommandResult(Generator::random_phrase(fortune_phrases),
                         Colors::pick_random_color());
  }
};
