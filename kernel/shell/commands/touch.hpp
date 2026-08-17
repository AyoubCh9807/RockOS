#pragma once

#include "../../storage/file_system.hpp"
#include "icommand.hpp"

class TouchCommand : public ICommand {
  FileSystem &fs;
  u32 &current_dir;

public:
  TouchCommand(FileSystem &fs, u32 &current_dir)
      : fs(fs), current_dir(current_dir) {}

  const char *name() const override { return "touch"; }

  CommandResult execute(int argc, char **argv) override {
    if (argc < 2)
      return CommandResult(Generator::random_phrase(touch_failure_phrases), Colors::RED);

    if (fs.touch(argv[1], current_dir))
      return CommandResult(Generator::random_phrase(touch_phrases), Colors::GREEN);

    return CommandResult(Generator::random_phrase(touch_failure_phrases), Colors::RED);
  }
};
