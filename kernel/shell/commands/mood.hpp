#pragma once
#include "../../data/personality.hpp"

#include "../../utils/terminal_utils.hpp"
#include "icommand.hpp"

class MoodCommand : public ICommand {

public:
  const char *name() const { return "mood"; }


  String execute(int argc, char **argv) {

    TerminalUtils::print(Generator::random_phrase(mood_descriptions));

    return "";
  }
};
