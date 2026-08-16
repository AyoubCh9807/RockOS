#pragma once
#include "../../data/ascii.hpp"

#pragma once

#include "../../utils/terminal_utils.hpp"
#include "icommand.hpp"

class AsciiCommand : public ICommand {

public:
  const char *name() const { return "ascii"; }

  String execute(int argc, char **argv) {

    TerminalUtils::print(Generator::random_phrase(ascii_art));
    return "";
  }
};


