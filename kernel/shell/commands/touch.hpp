#pragma once

#include "../../storage/file_system.hpp"
#include "icommand.hpp"

class TouchCommand : public ICommand {
  FileSystem &fs;

public:
  TouchCommand(FileSystem &fs) : fs(fs) {}

  const char *name() const override { return "touch"; }

  const char *execute(int argc, char **argv) override {
    if (argc < 2)
      return "usage: touch <name>\n\0";

    if (fs.touch(argv[1]))
      return "file created\n\0";

    return "failed creating file\n\0";
  }
};
