#pragma once

#include "../../tyrant_generator/generator.hpp"
#include "icommand.hpp"

class DamianCommand : public ICommand {

public:
  const char *name() const override { return "damian"; }

  const char *execute(int argc, char **argv) override {

    return Generator::generate_words();
  }
};
