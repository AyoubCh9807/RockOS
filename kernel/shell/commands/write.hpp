#pragma once
#include "../../data/system.hpp"
#include "../../storage/file_system.hpp"
#include "icommand.hpp"

class WriteCommand : public ICommand {

private:
  FileSystem &fs;
  u32 &current_dir;

public:
  WriteCommand(FileSystem &fs, u32 &current_dir)
      : fs(fs), current_dir(current_dir) {}

  const char *name() const override { return "write"; }

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

    if (fs.write_file(path, (const u8 *)content, StringUtils::strlen(content),
                      current_dir)) {

      return CommandResult(Generator::random_phrase(write_success_phrases),
                           Colors::GREEN);
    }

    return CommandResult(Generator::random_phrase(write_failure_phrases),
                         Colors::RED);
  }
};
