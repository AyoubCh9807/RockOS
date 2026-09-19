#pragma once
#include "../../data/system.hpp"
#include "../../desktop/wallpaper.hpp"
#include "icommand.hpp"

class WallpaperCommand : public ICommand {

public:
  const char *name() const { return "wallpaper"; }

  CommandResult execute(int argc, char **argv) {

    if (argc != 2)
      return CommandResult(Generator::random_phrase(foolish_phrases),
                           Colors::pick_random_color());

    if (StringUtils::strcmp(argv[1], "next") == 0) {
      Wallpaper::select_next_wallpaper();
      return CommandResult("the OS has obeyed", Colors::EMERALD);
    }
    if (StringUtils::strcmp(argv[1], "prev") == 0) {
      Wallpaper::select_prev_wallpaper();
      return CommandResult("Damian has obeyed", Colors::EMERALD);
    }
    if (StringUtils::strcmp(argv[1], "random") == 0) {
      Wallpaper::select_random_wallpaper();
      return CommandResult("Was that magic ?!Nope. just me..", Colors::EMERALD);
    }
    return CommandResult(Generator::random_phrase(foolish_phrases),
                         Colors::pick_random_color());
  }
};
