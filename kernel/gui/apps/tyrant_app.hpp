#pragma once

#include "../../data/system.hpp"
#include "../../tyrant_generator/generator.hpp"
#include "../window.hpp"
#include "../window_app.hpp"

class TyrantApp : public IWindowApp {
private:
  u32 fill_color = Colors::RED;
  const char *current_phrase = Generator::random_phrase(foolish_phrases);

public:
  const char *name() const override { return "Tyrant"; }

  void on_create(Window &win) override { draw(win); }

  void draw(Window &win) {
    win.clear(fill_color);
    win.clear(fill_color);
    win.draw_border(2, Colors::WHITE);
    win.draw_string(current_phrase, 50, 50, Colors::BLACK);
  }

  void on_draw(Window &win) override { draw(win); }

  void on_key(Window &win, const KeyEvent &ev) override {
    if (ev.scancode == (int)'r') {
      current_phrase = Generator::random_phrase(foolish_phrases);
    }
  }
};
