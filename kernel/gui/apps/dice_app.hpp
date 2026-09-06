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

    win.draw_border(2, Colors::WHITE);

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
