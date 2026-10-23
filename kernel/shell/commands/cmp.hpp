#pragma once
#include "icommand.hpp"

class CmpCommand : public ICommand {
public:
  

  const char *name() const { return "cmp"; }

  CommandResult execute(int argc, char **argv) {

    return CommandResult(StringUtils::strcmp(argv[1], argv[2]), Colors::WHITE);
  }
};