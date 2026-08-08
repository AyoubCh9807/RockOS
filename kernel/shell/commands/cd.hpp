#pragma once

#include "../../shared/types.hpp"
#include "../../storage/file_system.hpp"
#include "icommand.hpp"

class CdCommand : public ICommand {

private:
  FileSystem &fs;
  u32 &current_dir;

public:
  CdCommand(FileSystem &fs, u32 &current_dir)
      : fs(fs), current_dir(current_dir) {}

  const char *name() const override { return "cd"; }

  const char *execute(int argc, char **argv) override {

    if (argc < 2)
      return "usage: cd <path>";

    u32 inode = fs.resolve_path(argv[1], current_dir);

    if (inode == INVALID_INODE)
      return "path not found";

    current_dir = inode;

    return "";
  }
};
