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

  CommandResult execute(int argc, char **argv) override {

    if (argc < 2)
      return CommandResult(Generator::random_phrase(cd_failure_phrases),
                           Colors::RED);

    u32 inode = fs.resolve_path(argv[1], current_dir);

    if (inode == INVALID_INODE)
      return CommandResult(Generator::random_phrase(cd_failure_phrases),
                           Colors::RED);

    // *** ONE BIG BUG FIX: resolve_path() only rejects non-directories for
    // intermediate path components (it has to allow resolving to a file
    // as the final component, since ls/cat/rm all rely on that). `cd`
    // specifically requires the final target to be a directory, so check
    // it here instead. ***
    if (!fs.is_directory(inode))
      return CommandResult("Damian says this is not a directory, OBEY!",
                           Colors::RED);

    current_dir = inode;

    return CommandResult("");
  }
};
