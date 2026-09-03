#pragma once

#include "../../data/system.hpp"
#include "../../storage/file_system.hpp"
#include "icommand.hpp"
#include "../../utils/fs_utils.hpp"

class Basename : public ICommand {

public:

  const char *name() const override { return "basename"; }

  CommandResult execute(int argc, char **argv) {
    if (argc != 2)
      return CommandResult(Generator::random_phrase(foolish_phrases), Colors::RED);

    char path[256];
    FileSystem::copy_path(path, argv[1]);
    char* res = path;
    res = FSUtils::basename(path);

    char obuf[512];
    char* buf = obuf;
    StringUtils::snprintf(buf, sizeof(buf), "Damian found that the basename was: %s\n", res);

    return CommandResult(buf, Colors::GOLD);

  }
};

