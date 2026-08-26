#pragma once

#include "../../tyrant_generator/generator.hpp"
#include "../../data/characters.hpp"
#include "icommand.hpp"

class DamianCommand : public ICommand {

public:
  const char *name() const override { return "damian"; }

  CommandResult execute(int argc, char **argv) override {

    if(argc == 2 && StringUtils::strcmp(argv[1], "frame") == 0) 
      return CommandResult(damian_frame, Colors::RED);
    return CommandResult(Generator::generate_words(), Colors::RED);
  }
};
