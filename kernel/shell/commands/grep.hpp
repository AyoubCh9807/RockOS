#pragma once

#include "../../data/system.hpp"
#include "../../storage/file_system.hpp"
#include "../../storage/layout.hpp"
#include "icommand.hpp"

class GrepCommand : public ICommand {
  FileSystem &fs;
  u32 &current_dir;

public:
  GrepCommand(FileSystem &fs, u32 &current_dir)
      : fs(fs), current_dir(current_dir) {}

  const char *name() const override { return "grep"; }

  CommandResult execute(int argc, char **argv) override {
    if (argc < 3) {
      return CommandResult(Generator::random_phrase(foolish_phrases),
                           Colors::RED);
    }
    static u8 buffer[DIRECT_BLOCKS * BLOCK_SIZE + 1];
    size_t bytes_read = 0;

    if (!fs.read_file(argv[2], buffer, sizeof(buffer) - 1, bytes_read,
                      current_dir))
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

    while (buf[i] != '\0') {

      // Build the current line
      if (buf[i] != '\n') {
        line[line_index] = buf[i];
        line_index++;
      }

      // We reached the end of the line
      if (buf[i] == '\n' || buf[i + 1] == '\0') {

        line[line_index] = '\0';

        // Now  we search this line for argv[1]
        size_t j = 0;

        while (line[j] != '\0') {

          size_t k = 0;

          while (argv[1][k] != '\0' && line[j + k] == argv[1][k]) {
            k++;
          }

          if (argv[1][k] == '\0') {
            // Match
            for (size_t x = 0; line[x] != '\0'; x++) {
              cmd_output[output_index++] = line[x];
            }

            cmd_output[output_index++] = '\n';
            break;
          }

          j++;
        }

        // Reset for the next line
        line_index = 0;
      }

      i++;
    }

    cmd_output[output_index] = '\0';
    char output[DIRECT_BLOCKS * BLOCK_SIZE + 1];

    StringUtils::snprintf(output, sizeof(output),
                          "Tyrant kept searching over and over and here is "
                          "what he came up to:\n\n%s",
                          cmd_output);

    return CommandResult(output, 0xFFFFFF);
  }
};
