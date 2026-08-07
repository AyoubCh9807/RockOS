#pragma once

#include "../../storage/file_system.hpp"
#include "icommand.hpp"

class MkdirCommand : public ICommand {

  FileSystem &fs;

public:
  MkdirCommand(FileSystem &fs) : fs(fs) {}

  const char *name() const override { return "mkdir"; }

  const char *execute(int argc, char **argv) override {

    if (argc < 2)
      return "usage: mkdir <name>\n\0";

    if (fs.mkdir(argv[1]))
      return "directory created\n\0";

    return "failed creating directory\n\0";
  }
};
