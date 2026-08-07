#pragma once

#include "../../storage/file_system.hpp"
#include "icommand.hpp"

class RmCommand : public ICommand {
  FileSystem &fs;

public:
  RmCommand(FileSystem &fs) : fs(fs) {}

  const char *name() const override { return "rm"; }

  const char *execute(int argc, char **argv) override {
    if (argc < 2)
      return "usage: rm <name>\n\0";

    if (fs.delete_file(argv[1]))
      return "file removed\n\0";

    return "failed removing file\n\0";
  }
};
