#pragma once

#include "../../data/colors.hpp"
#include "../../data/shell_commands.hpp"
#include "../../tyrant_generator/generator.hpp"
#include "../../utils/terminal_utils.hpp"
#include "cli_app.hpp"

class PhraseViewerApp : public ICliApp {
private:

  TerminalUtils& terminal_utils;

  void draw() {
    auto term = terminal_utils; 

    term.clear();
    term.print("\n  ", 0xFFFFFF);
    term.print(Generator::random_phrase(reboot_phrases),
                Colors::pick_random_color());
    term.print("\n\n  press 'r' for another, 'q' to quit", 0x888888);
    term.render();

}

public:

  PhraseViewerApp(TerminalUtils& terminal_utils) : terminal_utils(terminal_utils) {}

  const char *name() const override { return "phrases"; }

  const char *quit_keyword() const override { return "q"; }

  bool wants_raw_keys() const { return true; }

  void on_enter() override { draw(); }

  bool handle_key(char c) {
    if (c == 'r')
      draw();

    // Any other key is just ignored, only 'r' and quit do anything.
    return true;
  }
};
