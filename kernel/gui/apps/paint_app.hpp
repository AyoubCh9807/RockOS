#pragma once

#include "../window.hpp"
#include "../window_app.hpp"

class PaintApp : public IWindowApp {
private:
  u32 fill_color = Colors::WHITE;
  u32 current_color = Colors::RED;

  struct Pixel {
    u32 x, y;
    u32 color;
    Pixel(u32 x, u32 y, u32 color) : x(x), y(y), color(color) {};
    Pixel() : x(0), y(0), color(Colors::WHITE) {};
  };

  Pixel pixel{};
  void move_up(Window &win) {
    if (pixel.y > 0) {
      pixel.y--;
    }
  }

  void move_down(Window &win) {
    if (pixel.y < win.height - 1) {
      pixel.y++;
    }
  }

  void move_left(Window &win) {
    if (pixel.x > 0) {
      pixel.x--;
    }
  }

  void move_right(Window &win) {
    if (pixel.x < win.width - 1) {
      pixel.x++;
    }
  }

  void clear_canvas(Window &win) { win.clear(fill_color); }

  void clear_pixel(Window &win) { win.set_pixel(pixel.x, pixel.y, fill_color); }

  void replace_pixel(Window &win, u32 color) {
    win.set_pixel(pixel.x, pixel.y, color);
  }

public:
  const char *name() const override { return "paint"; }

  void on_create(Window &win) override { draw(win); }

  void draw(Window &win) {
    win.clear(fill_color);

    win.draw_border(2, Colors::WHITE);
    win.set_pixel(pixel.x, pixel.y, current_color);
  }

  void on_draw(Window &win) override { draw(win); }

  void on_key(Window &win, const KeyEvent &ev) override {
    if (ev.scancode == (int)'a') {
      move_left(win);
    }
    if (ev.scancode == (int)'s') {
      move_down(win);
    }
    if (ev.scancode == (int)'w') {
      move_up(win);
    }
    if (ev.scancode == (int)'d') {
      move_right(win);
    }
    if (ev.scancode == (int)'x') {
      clear_pixel(win);
    }
    if (ev.scancode == (int)'c') {
      clear_canvas(win);
    }
    if (ev.scancode == (int)'r') {
      replace_pixel(win, current_color);
    }
    if (ev.scancode == (int)'k') {
      fill_color = Colors::pick_random_color();
    }
    draw(win);
  }
};
