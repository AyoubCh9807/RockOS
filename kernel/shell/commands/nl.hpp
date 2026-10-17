#pragma once

#include "../../storage/file_system.hpp"
#include "../../storage/layout.hpp"
#include "icommand.hpp"

class NlCommand : public ICommand {
  FileSystem &fs;
  u32 &current_dir;
  TerminalUtils &utils;

public:
  NlCommand(FileSystem &fs, u32 &current_dir, TerminalUtils &utils)
      : fs(fs), current_dir(current_dir), utils(utils) {}

  const char *name() const override { return "nl"; }

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

    int line = 1;
    utils.print_formatted(Colors::WHITE, "%d", line);
    utils.print("  ");

    for (size_t i = 0; i < bytes_read; i++) {
      utils.print_formatted(Colors::WHITE, "%d", buffer[i]);

      if (buffer[i] == '\n' && i + 1 < bytes_read) {
        line++;
        utils.print_formatted(Colors::WHITE, "%d", line);
        utils.print("  ");
      }
    }

    return CommandResult("");
  }
};
