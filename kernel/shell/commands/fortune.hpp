#pragma once
#include "../../data/fortune.hpp"

#pragma once

#include "../../utils/terminal_utils.hpp"
#include "icommand.hpp"

class FortuneCommand : public ICommand {

public:
  const char *name() const { return "fortune"; }

  String execute(int argc, char **argv) {

    TerminalUtils::print(Generator::random_phrase(fortune_phrases));
    return "";
  }
};
