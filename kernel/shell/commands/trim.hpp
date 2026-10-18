#pragma once
#include "../../data/system.hpp"
#include "icommand.hpp"

class TrimCommand : public ICommand {

public:
  const char *name() const { return "trim"; }

  CommandResult execute(int argc, char **argv) {

    if (argc < 2 || !argv[1])
      return CommandResult(Generator::random_phrase(foolish_phrases),
                           Colors::pick_random_color());
    char out[256];
    StringUtils::join(out, sizeof(out), argc, argv, " ");
    String str(out);
    return CommandResult(str.trim().substr(5));
  }
};
