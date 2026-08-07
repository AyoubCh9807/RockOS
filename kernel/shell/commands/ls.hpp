#pragma once

#include "../../shared/types.hpp"
#include "../../storage/file_system.hpp"
#include "../../utils/terminal_utils.hpp"
#include "icommand.hpp"

class LsCommand : public ICommand {

private:
  FileSystem &fs;

public:
  LsCommand(FileSystem &fs) : fs(fs) {}

  const char *name() const override { return "ls"; }

  const char *execute(int argc, char **argv) {

    if (argc == 1) {
      return fs.list_directory(ROOT_INODE);
    }

    u32 inode = fs.resolve_path(argv[1]);

    if (inode == INVALID_INODE) {
      TerminalUtils::print("directory not found\n");
      return "";
    }
    return fs.list_directory(inode);
  }
};
