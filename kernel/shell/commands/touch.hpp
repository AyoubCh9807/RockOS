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

  String execute(int argc, char **argv) override {
    if (argc < 2)
      return "usage: touch <name>\n\0";

    if (fs.touch(argv[1], current_dir))
      return "file created\n\0";

    return "failed creating file\n\0";
  }
};
