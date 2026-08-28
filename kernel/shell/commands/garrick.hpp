#pragma once

#include "../../data/characters.hpp"
#include "../../tyrant_generator/generator.hpp"
#include "icommand.hpp"

class GarrickCommand : public ICommand {

public:
  const char *name() const override { return "garrick"; }

  CommandResult execute(int argc, char **argv) override {

    if (argc == 2 && StringUtils::strcmp(argv[1], "frame") == 0)
      return CommandResult(garrick_frame, Colors::RED);
    return CommandResult(Generator::generate_words(), Colors::RED);
  }
};
