#pragma once

#include "../../storage/layout.hpp"
#include "icommand.hpp"
#include "../../data/system.hpp"

class YesCommand : public ICommand {
  TerminalUtils &utils;

public:
  YesCommand(TerminalUtils &utils) : utils(utils) {}

  const char *name() const override { return "yes"; }

  CommandResult execute(int argc, char **argv) override {
    if (argc > 2)
      return CommandResult(Generator::random_phrase(foolish_phrases),
                           Colors::RED);

    const char *str = argc == 1 ? "y" : argv[1];

    while (1) {
      utils.print(str);
    }

    return CommandResult("");
  }
};
