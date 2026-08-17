#pragma once

#include "../../tyrant_generator/generator.hpp"
#include "icommand.hpp"

class DamianCommand : public ICommand {

public:
  const char *name() const override { return "damian"; }

  CommandResult execute(int argc, char **argv) override {

    return CommandResult(Generator::generate_words(), Colors::RED);
  }
};
