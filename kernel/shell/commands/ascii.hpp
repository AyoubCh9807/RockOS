#pragma once
#include "../../data/ascii.hpp"

#pragma once

#include "../../utils/terminal_utils.hpp"
#include "icommand.hpp"

class AsciiCommand : public ICommand {

private:
  TerminalUtils &terminal_utils;

public:
  AsciiCommand(TerminalUtils &utils) : terminal_utils(utils) {}

  const char *name() const { return "ascii"; }

  CommandResult execute(int argc, char **argv) {

    return CommandResult(Generator::random_phrase(ascii_art),
                         Colors::pick_random_color());
  }
};
