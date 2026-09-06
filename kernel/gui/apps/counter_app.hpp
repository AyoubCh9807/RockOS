#pragma once

#include "../../data/system.hpp"
#include "../../tyrant_generator/generator.hpp"
#include "../window.hpp"
#include "../window_app.hpp"

class CounterApp : public IWindowApp {
private:
  u32 fill_color = Colors::BLACK;

  int count = 0;

public:
  const char *name() const override { return "Counter"; }

  void on_create(Window &win) override { draw(win); }

  void draw(Window &win) {
    win.clear(fill_color);

    win.draw_border(2, Colors::WHITE);
    char buf[64];
    StringUtils::snprintf(buf, sizeof(buf), "Your current count: %d", count);

    win.draw_string(buf, 100, 100, Colors::RED);
  }

  void on_draw(Window &win) override { draw(win); }

  void on_key(Window &win, const KeyEvent &ev) override {
    if (ev.scancode == (int)'+') {
      count++;
      draw(win);
    }
    if (ev.scancode == (int)'-') {
      count--;
      draw(win);
    }
  }
};
