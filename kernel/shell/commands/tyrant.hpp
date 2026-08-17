#pragma once

#include "../../tyrant_generator/generator.hpp"
#include "icommand.hpp"

class TyrantCommand : public ICommand {

public:
  const char *name() const override { return "tyrant"; }

  CommandResult execute(int argc, char **argv) override {

    return CommandResult(Generator::generate_words(), Colors::RED);
  }
};
