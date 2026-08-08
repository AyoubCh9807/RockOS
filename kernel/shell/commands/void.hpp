#pragma once
#include "../../data/personality.hpp"

#include "../../utils/terminal_utils.hpp"
#include "icommand.hpp"

class VoidCommand : public ICommand {

public:
  const char *name() const { return "void"; }

  String execute(int argc, char **argv) {

    //void_character_names
    for(int i = 0; i < VOID_HIDDEN_CHARACTER_COUNT; i++) {
      TerminalUtils::print(StringUtils::format("%s: %s\n", void_character_names[i], void_character_status[i]));
    }

    return "";
  }
};

