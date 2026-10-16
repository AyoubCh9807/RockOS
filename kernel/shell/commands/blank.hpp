#pragma once

#include "../../storage/file_system.hpp"
#include "../../storage/layout.hpp"
#include "icommand.hpp"

class BlankCommand : public ICommand {
  FileSystem &fs;
  u32 &current_dir;

public:
  BlankCommand(FileSystem &fs, u32 &current_dir)
      : fs(fs), current_dir(current_dir) {}

  const char *name() const override { return "blank"; }

  CommandResult execute(int argc, char **argv) override {
    if (argc < 2)
      return CommandResult(Generator::random_phrase(cat_failure_phrases),
                           Colors::RED);

    static u8 buffer[DIRECT_BLOCKS * BLOCK_SIZE + 1];
    size_t bytes_read = 0;

    if (!fs.read_file(argv[1], buffer, sizeof(buffer) - 1, bytes_read,
                      current_dir))
      return CommandResult(Generator::random_phrase(cat_failure_phrases),
                           Colors::RED);

    buffer[bytes_read] = '\0';

    int blanks = 0;

    for (size_t i = 1; i < bytes_read; i++) {
      if (buffer[i - 1] == '\n' && buffer[i] == '\n')
        blanks++;
    }

    char output[128];

    StringUtils::snprintf(
        output, sizeof(output),
        "Damian counted the blank lines in %s and they appear to be: %d",
        argv[1], blanks);

    return CommandResult(output, Colors::GREEN);
  }
};