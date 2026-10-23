#pragma once

#include "../../data/system.hpp"
#include "../../storage/file_system.hpp"
#include "../../storage/layout.hpp"
#include "../../data/colors.hpp"
#include "icommand.hpp"

class ColorsCommand : public ICommand {
private:
  FileSystem &fs;
  u32 &current_dir;

public:
  ColorsCommand(FileSystem &fs, u32 &current_dir)
      : fs(fs), current_dir(current_dir) {}

  const char *name() const override { return "colors"; }

  CommandResult execute(int argc, char **argv) override {
    char out[1024] = "";
    int offset = 0;

    offset += StringUtils::snprintf(out + offset, sizeof(out) - offset,
                                    "RockOS Color Palette:\n");

    for (int i = 0; i < Colors::PALETTE_SIZE; i++) {
      u32 color = Colors::PALETTE[i];

      if (color == Colors::BLACK)
        continue;

      offset += StringUtils::snprintf(out + offset, sizeof(out) - offset,
                                      "[Color #%02d]: 0x%06X\n", i, color);

      if (sizeof(out) - offset < 64)
        break;
    }

    return CommandResult(out, Colors::CYAN);
  }
};
