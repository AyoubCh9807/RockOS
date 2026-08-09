#pragma once
#include "../../data/personality.hpp"

#include "../../utils/list_utils.hpp"
#include "../../utils/terminal_utils.hpp"
#include "icommand.hpp"

class MoodCommand : public ICommand {

public:
  const char *name() const { return "mood"; }

  String execute(int argc, char **argv) {

    TerminalUtils::print(ListUtils::get_random_string(
        mood_damian_reactions, mood_tyrant_reactions, mood_descriptions,
        mood_names));

    return "";
  }
};
