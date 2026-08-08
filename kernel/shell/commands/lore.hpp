#pragma once
#include "../../data/lore.hpp"

#include "../../utils/terminal_utils.hpp"
#include "icommand.hpp"

class LoreCommand : public ICommand {

public:
  const char *name() const { return "lore"; }

  String execute(int argc, char **argv) {

    //void_character_names
    TerminalUtils::print(Generator::random_phrase(damian_lore));
    return "";
  }
};


