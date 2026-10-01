#pragma once

#include "../../data/system.hpp"
#include "../../storage/file_system.hpp"
#include "../../storage/layout.hpp"
#include "icommand.hpp"

class TailCommand : public ICommand {
  FileSystem &fs;
  u32 &current_dir;

public:
  TailCommand(FileSystem &fs, u32 &current_dir)
      : fs(fs), current_dir(current_dir) {}

  const char *name() const override { return "tail"; }

  CommandResult execute(int argc, char **argv) override {
    if (argc < 2) {
      return CommandResult(Generator::random_phrase(foolish_phrases),
                           Colors::RED);
    }

    int requested_line_count = 10;
    int file_arg = 1;

    if (StringUtils::strcmp(argv[1], "-n") == 0) {
      if (argc < 4) {
        return CommandResult(Generator::random_phrase(foolish_phrases),
                             Colors::RED);
      }

      requested_line_count = StringUtils::to_int(argv[2]);
      file_arg = 3;
    }

    static char buffer[DIRECT_BLOCKS * BLOCK_SIZE + 1];
    size_t bytes_read = 0;

    if (!fs.read_file(argv[file_arg], (u8 *)buffer, sizeof(buffer) - 1,
                      bytes_read, current_dir)) {
      return CommandResult(Generator::random_phrase(foolish_phrases),
                           Colors::RED);
    }

    buffer[bytes_read] = '\0';

    int line_count = 0;

    for (int i = 0; i < (int)bytes_read; i++) {
      if (buffer[i] == '\n')
        line_count++;
    }

    int target_line = line_count - requested_line_count;

    if (target_line < 0)
      target_line = 0;

    int start = 0;
    int current_line = 0;

    while (current_line < target_line && buffer[start] != '\0') {
      if (buffer[start] == '\n')
        current_line++;

      start++;
    }

    char cmd_output[DIRECT_BLOCKS * BLOCK_SIZE + 1];
    cmd_output[0] = '\0';

    StringUtils::append(cmd_output, buffer + start);

    char output[DIRECT_BLOCKS * BLOCK_SIZE + 1];

    StringUtils::snprintf(
        output, sizeof(output),
        "Damian grabbed the file by its tail and found:\n\n%s", cmd_output);

    return CommandResult(output, 0xFFFFFF);
  }
};
