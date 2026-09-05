#pragma once


#include "../../data/system.hpp"
#include "../../random/random.hpp"
#include "../../tyrant_generator/generator.hpp"
#include "../window.hpp"
#include "../window_app.hpp"

class DiceApp : public IWindowApp {
private:
  u32 fill_color = Colors::RED;
  int current_roll = roll();
  bool rolled_once = false;

  int roll() { return (Random::next() % 6) + 1; }

public:
  const char *name() const override { return "Dice"; }

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

    char buf[128];
    StringUtils::snprintf(buf, sizeof(buf), "Your current roll is %d",
                          current_roll);

    if (!rolled_once) {
      win.draw_string("Welcome to the dice app! Press 'R' to roll!", 100, 100,
                      Colors::GOLD);
    } else {
      win.draw_string("Press 'R' to roll again!", 100, 100, Colors::EMERALD);
    }

    win.draw_string(buf, 50, 50, Colors::GOLD);
  }

  void on_draw(Window &win) override { draw(win); }

  void on_key(Window &win, const KeyEvent &ev) override {
    if (ev.scancode == (int)'r') {
      current_roll = (Random::next() % 6) + 1;
      rolled_once = true;
      draw(win);
    }
  }
};
