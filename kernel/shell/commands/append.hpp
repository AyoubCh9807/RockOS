#pragma once

#include "../../data/system.hpp"
#include "../../storage/file_system.hpp"
#include "icommand.hpp"

class AppendCommand : public ICommand {

private:
  FileSystem &fs;
  u32 &current_dir;

public:
  AppendCommand(FileSystem &fs, u32 &current_dir)
      : fs(fs), current_dir(current_dir) {}

  const char *name() const override { return "append"; }

  CommandResult execute(int argc, char **argv) {
    if (argc < 3)
      return CommandResult("");

    char path[256];
    FileSystem::copy_path(path, argv[1]);

    char content[1024];
    content[0] = '\0';

    for (int i = 2; i < argc; i++) {
      if (i > 2)
        StringUtils::append(content, " ");

      StringUtils::append(content, argv[i]);
    }

    static u8 buffer[DIRECT_BLOCKS * BLOCK_SIZE + 1];

    size_t bytes_read = 0;

    // Read the existing file.
    if (!fs.read_file(path, buffer, sizeof(buffer) - 1, bytes_read,
                      current_dir)) {
      return CommandResult(Generator::random_phrase(write_failure_phrases),
                           Colors::RED);
    }

    size_t content_length = StringUtils::strlen(content);

    // Make sure the combined file fits.
    if (bytes_read + content_length > sizeof(buffer) - 1) {
      return CommandResult("Damian found that the file is too large.",
                           Colors::RED);
    }
    if (bytes_read > 0)
      buffer[bytes_read++] = '\n';

    for (size_t i = 0; i < content_length; i++) {
      buffer[bytes_read + i] = content[i];
    }

    bytes_read +=
        content_length; // Rewrite the file with the combined contents.
    if (fs.write_file(path, buffer, bytes_read, current_dir)) {
      return CommandResult(Generator::random_phrase(write_success_phrases),
                           Colors::GREEN);
    }

    return CommandResult(Generator::random_phrase(write_failure_phrases),
                         Colors::RED);
  }
};
