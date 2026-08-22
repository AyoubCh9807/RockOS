#pragma once

#include "../../data/system.hpp"
#include "../../storage/file_system.hpp"
#include "../../storage/layout.hpp"
#include "icommand.hpp"

class WcCommand : public ICommand {
  FileSystem &fs;
  u32 &current_dir;

public:
  WcCommand(FileSystem &fs, u32 &current_dir)
      : fs(fs), current_dir(current_dir) {}

  const char *name() const override { return "wc"; }

  CommandResult execute(int argc, char **argv) override {
    if (argc < 2) {
      return CommandResult(Generator::random_phrase(foolish_phrases),
                           Colors::RED);
    }
    static u8 buffer[DIRECT_BLOCKS * BLOCK_SIZE + 1];
    size_t bytes_read = 0;

    if (!fs.read_file(argv[1], buffer, sizeof(buffer) - 1, bytes_read,
                      current_dir))
      return CommandResult(Generator::random_phrase(foolish_phrases),
                           Colors::RED);

    buffer[bytes_read] = '\0';

    const char *buf = (const char *)buffer;
    int i = 0;
    size_t count = 0;
    while (buf[i] != '\0') {
      if (buf[i] == ' ')
        count++;
      i++;
    }
    char output[128];

    StringUtils::snprintf(
        output, sizeof(output),
        "Damian counted the words himself and they appeared to be %d", count);

    return CommandResult(output, 0xFFFFFF);
  }
};
