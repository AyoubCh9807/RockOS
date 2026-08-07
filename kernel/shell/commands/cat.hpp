#pragma once

#include "../../storage/file_system.hpp"
#include "../../utils/terminal_utils.hpp"
#include "icommand.hpp"

class CatCommand : public ICommand {
  FileSystem &fs;

public:
  CatCommand(FileSystem &fs) : fs(fs) {}

  const char *name() const override { return "cat"; }

  const char *execute(int argc, char **argv) override {
    if (argc < 2)
      return "usage: cat <name>\n\0";

    static u8 buffer[DIRECT_BLOCKS * BLOCK_SIZE + 1];
    size_t bytes_read = 0;

    if (!fs.read_file(argv[1], buffer, sizeof(buffer) - 1, bytes_read))
      return "failed reading file\n\0";

    buffer[bytes_read] = '\0';

    return (const char *)buffer;
  }
};
