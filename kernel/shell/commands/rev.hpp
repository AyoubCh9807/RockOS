#pragma once

#include "../../data/system.hpp"
#include "icommand.hpp"

class RevCommand : public ICommand {

public:

  const char *name() const override { return "rev"; }

  CommandResult execute(int argc, char **argv) {
    if (argc != 2)
      return CommandResult(Generator::random_phrase(foolish_phrases), Colors::RED);

    char buf[256];

    StringUtils::reverse(argv[1], buf);

    char obuf[512];
  
    StringUtils::snprintf(obuf, sizeof(obuf), "Tyrant found that the reverse was: %s\n", buf);

    return CommandResult(obuf, Colors::GOLD);

  }
};


