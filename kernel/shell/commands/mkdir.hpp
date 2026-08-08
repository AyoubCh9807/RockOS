#pragma once

#include "../../storage/file_system.hpp"
#include "icommand.hpp"

class MkdirCommand : public ICommand {

  FileSystem &fs;
  u32 &current_dir;

public:
  MkdirCommand(FileSystem &fs, u32 &current_dir)
      : fs(fs), current_dir(current_dir) {}

  const char *name() const override { return "mkdir"; }

  String execute(int argc, char **argv) override {

    if (argc < 2) {
      return Generator::random_phrase(command_not_found_phrases);
    }
    if (fs.mkdir(argv[1], current_dir)) {
      return Generator::random_phrase(mkdir_phrases);
    }

    return "failed creating directory\n\0";
  }
};
