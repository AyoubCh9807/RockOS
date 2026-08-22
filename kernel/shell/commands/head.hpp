#pragma once

#include "../../data/system.hpp"
#include "../../storage/file_system.hpp"
#include "../../storage/layout.hpp"
#include "icommand.hpp"

class HeadCommand : public ICommand {
  FileSystem &fs;
  u32 &current_dir;

public:
  HeadCommand(FileSystem &fs, u32 &current_dir)
      : fs(fs), current_dir(current_dir) {}

  const char *name() const override { return "head"; }

  CommandResult execute(int argc, char **argv) override {
    if (argc < 2) {
      return CommandResult(Generator::random_phrase(foolish_phrases),
                           Colors::RED);
    }

    int requested_line_count = 10;
    if (StringUtils::strcmp(argv[1], "-n") == 0) {
      if (argc < 4) {
        return CommandResult(Generator::random_phrase(foolish_phrases),
                             Colors::RED);
      }
      requested_line_count = StringUtils::to_int(argv[2]);
    }

    static u8 buffer[DIRECT_BLOCKS * BLOCK_SIZE + 1];
    size_t bytes_read = 0;

    int argv_to_read = StringUtils::strcmp(argv[1], "-n") == 0 ? 3 : 1;

    if (!fs.read_file(argv[argv_to_read], buffer, sizeof(buffer) - 1,
                      bytes_read, current_dir))
      return CommandResult(Generator::random_phrase(foolish_phrases),
                           Colors::RED);

    buffer[bytes_read] = '\0';

    const char *buf = (const char *)buffer;

    char line[128];
    int i = 0;
    int count = 0;
    int line_index = 0;

    char cmd_output[DIRECT_BLOCKS * BLOCK_SIZE + 1];
    size_t output_index = 0;

    while (buf[i] != '\0' && count < requested_line_count) {

      // Build the current line
      if (buf[i] != '\n') {
        line[line_index] = buf[i];
        line_index++;
      }

      // We reached the end of the line
      if (buf[i] == '\n' || buf[i + 1] == '\0') {
        line[line_index] = '\0';

        for (int j = 0; j < line_index; j++) {
          cmd_output[output_index++] = line[j];
        }

        cmd_output[output_index++] = '\n';

        line_index = 0;
        count++;
      }

      i++;
    }

    cmd_output[output_index] = '\0';
    char output[DIRECT_BLOCKS * BLOCK_SIZE + 1];

    StringUtils::snprintf(
        output, sizeof(output),
        "Damian grabbed the file by its head and found:\n\n%s", cmd_output);

    return CommandResult(output, 0xFFFFFF);
  }
};
