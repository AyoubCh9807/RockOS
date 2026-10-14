```cpp
#pragma once

#include "../../data/system.hpp"
#include "../../storage/file_system.hpp"
#include "../../storage/layout.hpp"
#include "icommand.hpp"

class CutCommand : public ICommand {
  FileSystem &fs;
  u32 &current_dir;

public:
  CutCommand(FileSystem &fs, u32 &current_dir)
      : fs(fs), current_dir(current_dir) {}

  const char *name() const override { return "cut"; }

  CommandResult execute(int argc, char **argv) override {
    if (argc < 4)
      return CommandResult(Generator::random_phrase(foolish_phrases),
                           Colors::RED);

    static u8 buffer[DIRECT_BLOCKS * BLOCK_SIZE + 1];
    size_t bytes_read = 0;

    if (!fs.read_file(argv[1], buffer, sizeof(buffer) - 1, bytes_read,
                      current_dir))
      return CommandResult(Generator::random_phrase(foolish_phrases),
                           Colors::RED);

    buffer[bytes_read] = '\0';

    int start = StringUtils::to_int(argv[2]);
    int end = StringUtils::to_int(argv[3]);

    if (start < 0 || start >= end || (size_t)end > bytes_read)
      return CommandResult(Generator::random_phrase(foolish_phrases),
                           Colors::RED);

    char outputb[128];
    size_t length = (size_t)(end - start);

    if (length >= sizeof(outputb))
      return CommandResult(Generator::random_phrase(foolish_phrases),
                           Colors::RED);

    for (size_t i = 0; i < length; i++)
      outputb[i] = buffer[start + i];

    outputb[length] = '\0';

    char output[256];

    StringUtils::snprintf(
        output, sizeof(output),
        "Damian cut the file content as you asked resulting in: %s",
        outputb);

    return CommandResult(output, 0xFFFFFF);
  }
};
```
