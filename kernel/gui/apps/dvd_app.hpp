#pragma once

#include "../../data/system.hpp"
#include "../../random/random.hpp"
#include "../../tyrant_generator/generator.hpp"
#include "../window.hpp"
#include "../window_app.hpp"

class DvdApp : public IWindowApp {
private:
  u32 fill_color = Colors::RED;
  u32 string_color = Colors::GOLD;
  const char *bouncing_string = "Rock OS!";
  int x = 0;
  int y = 0;

  int dx = 5;
  int dy = 5;

  void update_bouncing_string(Window &win) {
    int limit_x = win.width - StringUtils::strlen(bouncing_string) *
                                  Graphics::CHARACTER_WIDTH;

    int limit_y = win.height - Graphics::CHARACTER_HEIGHT;

    x += dx;
    y += dy;

    if (x <= 0) {
      x = 0;
      dx = -dx;
    }

    if (x >= limit_x) {
      x = limit_x;
      dx = -dx;
    }

    if (y <= 0) {
      y = 0;
      dy = -dy;
    }

    if (y >= limit_y) {
      y = limit_y;
      dy = -dy;
    }
  }

public:
  const char *name() const override { return "DVD"; }

  void on_create(Window &win) override { draw(win); }

  void draw(Window &win) {
    win.clear(fill_color);

    win.draw_border(2, Colors::WHITE);

    win.draw_string(bouncing_string, x, y, string_color);
  }

  void on_draw(Window &win) override {
    update_bouncing_string(win);
    draw(win);
  }

  void on_key(Window &win, const KeyEvent &ev) override {}
};
