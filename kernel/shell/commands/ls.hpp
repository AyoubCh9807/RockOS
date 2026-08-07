#pragma once

#include "../../shared/types.hpp"
#include "../../storage/file_system.hpp"
#include "../../utils/terminal_utils.hpp"
#include "icommand.hpp"

class LsCommand : public ICommand {

private:
  FileSystem &fs;
  u32 &current_dir;

public:
  LsCommand(FileSystem &fs, u32 &current_dir)
      : fs(fs), current_dir(current_dir) {}

  const char *name() const override { return "ls"; }

  const char *execute(int argc, char **argv) {

    TerminalUtils::print("CURRENT DIR: ");
    TerminalUtils::print_number(current_dir);
    TerminalUtils::print("\n");

    if (argc == 1) {
      return fs.list_directory(current_dir);
    }

    u32 inode = fs.resolve_path(argv[1]);

    if (inode == INVALID_INODE) {
      TerminalUtils::print("directory not found\n");
      return "";
    }
    return fs.list_directory(inode);
  }
};
