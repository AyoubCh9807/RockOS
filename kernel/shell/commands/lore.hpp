#pragma once
#include "../../data/lore.hpp"

#include "../../utils/list_utils.hpp"
#include "../../utils/terminal_utils.hpp"
#include "icommand.hpp"

class LoreCommand : public ICommand {

public:
  const char *name() const { return "lore"; }

  CommandResult execute(int argc, char **argv) {

    return CommandResult(
        ListUtils::get_random_string(damian_lore, tyrant_lore, rockfs_lore),
        Colors::pick_random_color());
  }
};
