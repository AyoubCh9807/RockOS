#pragma once

#include "../../tyrant_generator/generator.hpp"
#include "icommand.hpp"

class TyrantCommand : public ICommand {

public:
  const char *name() const override { return "tyrant"; }

  String execute(int argc, char **argv) override {

    return Generator::generate_words();
  }
};
