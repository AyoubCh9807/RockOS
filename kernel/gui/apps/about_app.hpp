#pragma once

#include "../window.hpp"
#include "../window_app.hpp"

class AboutApp : public IWindowApp {
private:
  static constexpr u32 FILL_COLOR = Colors::DARK_GRAY;
  static constexpr u32 TEXT_COLOR = Colors::WHITE;
  static constexpr u32 MUTED_COLOR = Colors::LIGHT_GRAY;
  static constexpr u32 ACCENT_COLOR = Colors::RED;
  static constexpr u32 GOLD_COLOR = Colors::GOLD;

public:
  const char *name() const override { return "About"; }

  void on_create(Window &win) override { draw(win); }

  void draw(Window &win) {
    win.clear(FILL_COLOR);

    const u32 width = win.get_width();
    const u32 center = width / 2;

    // Header
    win.draw_string("ROCK OS", 40, 35, ACCENT_COLOR);
    win.draw_string("About Rock OS", 40, 65, TEXT_COLOR);

    // Separator
    win.draw_rect(40, 95, width - 80, 2, ACCENT_COLOR);

    // Description
    win.draw_string("A tiny operating system built from scratch.",
                    40, 120, TEXT_COLOR);
    win.draw_string("Written in C++ with a little bit of attitude.",
                    40, 142, MUTED_COLOR);

    // Version
    win.draw_string("VERSION", 40, 185, GOLD_COLOR);
    win.draw_string("0.1.0", 40, 207, TEXT_COLOR);

    // System
    win.draw_string("SYSTEM", 40, 250, GOLD_COLOR);
    win.draw_string("Architecture: x86_64", 40, 272, TEXT_COLOR);
    win.draw_string("Kernel: Rock OS", 40, 294, TEXT_COLOR);
    win.draw_string("Language: C++", 40, 316, TEXT_COLOR);

    // Philosophy
    win.draw_string("BUILT FROM SCRATCH", 40, 360, GOLD_COLOR);
    win.draw_string("Bootloader", 40, 382, MUTED_COLOR);
    win.draw_string("Kernel", 160, 382, MUTED_COLOR);
    win.draw_string("Memory", 40, 404, MUTED_COLOR);
    win.draw_string("Graphics", 160, 404, MUTED_COLOR);
    win.draw_string("Filesystem", 40, 426, MUTED_COLOR);
    win.draw_string("Desktop", 160, 426, MUTED_COLOR);

    // Footer
    win.draw_rect(40, 460, width - 80, 1, ACCENT_COLOR);
    win.draw_string("Made with Rock.", 40, 480, GOLD_COLOR);
    win.draw_string("Small OS. Loud personality.", 40, 502, MUTED_COLOR);
  }

  void on_draw(Window &win) override { draw(win); }

  void on_key(Window &win, const KeyEvent &ev) override {
    // Nothing for now.
  }

  void on_mouse_event(MouseEvent &ev)  {
    // Nothing for now.
  }
};
