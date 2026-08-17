#pragma once

#include "../../shared/types.hpp"
#include "../../storage/file_system.hpp"
#include "../../utils/terminal_utils.hpp"
#include "icommand.hpp"

#include "../../data/command_descriptions.hpp"

class LsCommand : public ICommand {

private:
  FileSystem &fs;
  u32 &current_dir;

public:
  LsCommand(FileSystem &fs, u32 &current_dir)
      : fs(fs), current_dir(current_dir) {}

  const char *name() const override { return "ls"; }


  CommandResult execute (int argc, char **argv) override {

    if (argc == 1) {
      return CommandResult(String(fs.list_directory(current_dir)), Colors::WHITE);
    }

    u32 inode = fs.resolve_path(argv[1], current_dir);

    if (inode == INVALID_INODE) {
      return CommandResult(Generator::random_phrase(command_not_found_phrases), Colors::RED);
    }
    return CommandResult(fs.list_directory(inode), Colors::WHITE);
  }
};
