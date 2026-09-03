#pragma once

#include "window.hpp"
#include "window_app.hpp"
#include "../tyrant_generator/generator.hpp"
#include "../data/system.hpp"

class TyrantApp : public IWindowApp {
private:
  u32 fill_color = Colors::RED;
  const char* current_phrase = Generator::random_phrase(foolish_phrases);

public:
  const char *name() const override { return "Tyrant"; }

  void on_create(Window &win) override { draw(win); }

  void draw(Window &win) {
    win.clear(fill_color);

    // 2px white border, just to prove per-pixel drawing works.
    for (int x = 0; x < win.width; x++) {
      win.set_pixel(x, 0, 0xFFFFFF);
      win.set_pixel(x, 1, 0xFFFFFF);
      win.set_pixel(x, win.height - 1, 0xFFFFFF);
      win.set_pixel(x, win.height - 2, 0xFFFFFF);
    }
    for (int y = 0; y < win.height; y++) {
      win.set_pixel(0, y, 0xFFFFFF);
      win.set_pixel(1, y, 0xFFFFFF);
      win.set_pixel(win.width - 1, y, 0xFFFFFF);
      win.set_pixel(win.width - 2, y, 0xFFFFFF);
    }


    win.draw_string(current_phrase, 50, 50, Colors::BLACK);

  }

  void on_draw(Window &win) override { draw(win); }

  void on_key(Window &win, const KeyEvent &ev) override {
    if(ev.scancode == (int)'r') {
      current_phrase = Generator::random_phrase(foolish_phrases);
    }
  }
};

