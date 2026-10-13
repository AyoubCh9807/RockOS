#pragma once

#include "../../storage/file_system.hpp"
#include "../../storage/layout.hpp"
#include "icommand.hpp"

class TacCommand : public ICommand {
  FileSystem &fs;
  u32 &current_dir;

public:
  TacCommand(FileSystem &fs, u32 &current_dir)
      : fs(fs), current_dir(current_dir) {}

  const char *name() const override { return "tac"; }

  CommandResult execute(int argc, char **argv) override {
    if (argc < 2) {
      return CommandResult(Generator::random_phrase(cat_failure_phrases),
                           Colors::RED);
    }
    static u8 buffer[DIRECT_BLOCKS * BLOCK_SIZE + 1];
    static u8 reverse_buffer[DIRECT_BLOCKS * BLOCK_SIZE + 1];
    size_t bytes_read = 0;

    if (!fs.read_file(argv[1], buffer, sizeof(buffer) - 1, bytes_read,
                      current_dir))
      return CommandResult(Generator::random_phrase(cat_failure_phrases),
                           Colors::RED);

    buffer[bytes_read] = '\0';
    "rock\nmetal\ncool\n"
    "will become"
    "cool\nmetal\nrock\0";

    size_t out = 0;
    size_t end = bytes_read;

    while (end > 0) {
      size_t start = end;

      while (start > 0 && buffer[start - 1] != '\n')
        start--;

      for (size_t j = start; j < end; j++)
        reverse_buffer[out++] = buffer[j];

      if (start > 0)
        reverse_buffer[out++] = '\n';

      end = start > 0 ? start - 1 : 0;
    }

    reverse_buffer[out] = '\0';

    reverse_buffer[bytes_read] = '\0';

    return CommandResult((const char *)reverse_buffer);
  }
};
