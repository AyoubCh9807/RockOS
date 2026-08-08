#pragma once

#include "icommand.hpp"
#include "../../utils/terminal_utils.hpp"

class EchoCommand : public ICommand {

public:
  const char *name() const  { return "echo"; }

  String execute(int argc, char **argv)  {

    for (int i = 1; i < argc; i++) {

      TerminalUtils::print(argv[i]);

      if (i != argc - 1)
        TerminalUtils::print(" ");
    }

    TerminalUtils::print("\n");

    return "";
  }
};
