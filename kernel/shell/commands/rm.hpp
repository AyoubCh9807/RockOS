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

  String execute(int argc, char **argv) override {
    if (argc < 2)
      return "usage: rm <name>\n\0";
    if (fs.delete_file(argv[1], current_dir))
      return "file removed\n\0";
    return "failed removing file\n\0";
  }
};
