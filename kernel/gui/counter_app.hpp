#pragma once

#include "../data/system.hpp"
#include "../tyrant_generator/generator.hpp"
#include "window.hpp"
#include "window_app.hpp"

class CounterApp : public IWindowApp {
private:
  u32 fill_color = Colors::BLACK;

  int count = 0;

public:
  const char *name() const override { return "counter"; }

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
