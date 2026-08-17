#pragma once
#include "../../data/personality.hpp"

#include "../../utils/list_utils.hpp"
#include "../../utils/terminal_utils.hpp"
#include "icommand.hpp"

class MoodCommand : public ICommand {

public:
  const char *name() const { return "mood"; }

  CommandResult execute(int argc, char **argv) {

    return CommandResult(ListUtils::get_random_string(mood_damian_reactions,
                                         mood_tyrant_reactions,
                                         mood_descriptions, mood_names), Colors::RED);

  }
};
