#pragma once

#include "../window.hpp"
#include "../window_app.hpp"

class DemoApp : public IWindowApp {
private:
  u32 fill_color = 0x3050A0;

public:
  const char *name() const override { return "demo"; }

  void on_create(Window &win) override { draw(win); }

  void draw(Window &win) {
    win.clear(fill_color);

    win.draw_border(2, Colors::WHITE);
  }

  void on_draw(Window &win) override { draw(win); }

  void on_key(Window &win, const KeyEvent &ev) override {
    // Any keypress cycles the fill color, just to prove input routing works.
    fill_color = (fill_color == 0x3050A0) ? 0xA05030 : 0x3050A0;
  }
};
