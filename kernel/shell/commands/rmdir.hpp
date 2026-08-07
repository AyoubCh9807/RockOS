#pragma once

#include "../../storage/file_system.hpp"
#include "icommand.hpp"

class RmdirCommand : public ICommand {
  FileSystem &fs;

public:
  RmdirCommand(FileSystem &fs) : fs(fs) {}

  const char *name() const override { return "rmdir"; }

  const char *execute(int argc, char **argv) override {
    if (argc < 2)
      return "usage: rmdir <name>\n\0";

    if (fs.remove_directory(argv[1]))
      return "directory removed\n\0";

    return "failed removing directory (not found, not a dir, or not empty)\n\0";
  }
};
