#pragma once

#include "../../storage/file_system.hpp"
#include "icommand.hpp"
class PwdCommand : public ICommand {

  FileSystem &fs;

  u32 &current_dir;

public:
  PwdCommand(FileSystem &fs, u32 &dir) : fs(fs), current_dir(dir) {}

  const char *name() const override { return "pwd"; }

  const char *execute(int argc, char **argv) override {
    return fs.get_path(current_dir);
  }
};
