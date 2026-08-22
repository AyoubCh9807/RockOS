#pragma once
#include "../../data/personality.hpp"

#include "../../utils/terminal_utils.hpp"
#include "icommand.hpp"

class VoidCommand : public ICommand {
private:
  TerminalUtils &terminal_utils;

public:
  VoidCommand(TerminalUtils &utils) : terminal_utils(utils) {};

  const char *name() const { return "void"; }

  CommandResult execute(int argc, char **argv) {

    // void_character_names
    for (int i = 0; i < VOID_HIDDEN_CHARACTER_COUNT; i++) {
      terminal_utils.print_formatted(0xFFFFFF, "%s: %s\n",
                                     void_character_names[i],
                                     void_character_status[i]),
          Colors::pick_random_color();
    }

    return CommandResult("");
  }
};
