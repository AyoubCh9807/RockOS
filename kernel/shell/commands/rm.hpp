#pragma once

#include "../../storage/file_system.hpp"
#include "icommand.hpp"

class RmCommand : public ICommand {
  FileSystem &fs;
  u32 &current_dir;

public:
  RmCommand(FileSystem &fs, u32 &current_dir)
      : fs(fs), current_dir(current_dir) {}

  const char *name() const override { return "rm"; }

  CommandResult execute(int argc, char **argv) override {
    if (argc < 2)
      return CommandResult(Generator::random_phrase(rm_failure_phrases),
                           Colors::RED);
    if (fs.delete_file(argv[1], current_dir))
      return CommandResult(Generator::random_phrase(rm_phrases), Colors::GREEN);
    return CommandResult(Generator::random_phrase(rm_failure_phrases),
                         Colors::RED);
  }
};
