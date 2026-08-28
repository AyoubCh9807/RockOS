#pragma once
#include "../../data/fortune.hpp"
#include "icommand.hpp"

class RandomCommand : public ICommand {

public:
  const char *name() const { return "random"; }

  CommandResult execute(int argc, char **argv) {

    return CommandResult(StringUtils::iota(Random::next()),
                         Colors::pick_random_color());
  }
};
