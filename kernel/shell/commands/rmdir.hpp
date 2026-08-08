#pragma once

#include "../../storage/file_system.hpp"
#include "icommand.hpp"

class RmdirCommand : public ICommand {
  FileSystem &fs;
  u32 &current_dir;

public:
  RmdirCommand(FileSystem &fs, u32 &current_dir) : fs(fs), current_dir(current_dir) {}

  const char *name() const override { return "rmdir"; }

  const char *execute(int argc, char **argv) {

    if (FS_DEBUG)
      TerminalUtils::print("RMDIR START\n");

    if (argc < 2)
      return "usage\n";

    if (FS_DEBUG) {
      TerminalUtils::print("REMOVING: ");
      TerminalUtils::print(argv[1]);
      TerminalUtils::print("\n");
    }
    bool result = fs.remove_directory(argv[1], current_dir);

    if (FS_DEBUG)
      TerminalUtils::print("RMDIR END\n");

    if (result)
      return "removed\n";

    return "failed\n";
  }
};
