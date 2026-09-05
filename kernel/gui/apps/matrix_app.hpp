#pragma once

#include "../window.hpp"
#include "../window_app.hpp"

class MatrixApp : public IWindowApp {
private:
  u32 fill_color = Colors::BLACK;
  
  // Assume a safe maximum number of columns for your screen resolution
  static constexpr int MAX_COLS = 256;
  int drops[MAX_COLS];
  bool initialized = false;

public:
  const char *name() const override { return "Matrix"; }

  void on_create(Window &win) override {
    init_drops(win);
    draw(win); 
  }

  void init_drops(Window &win) {
    int cols = win.width / Graphics::CHARACTER_WIDTH;
    if (cols > MAX_COLS) cols = MAX_COLS; // safety check

    for (int i = 0; i < cols; i++) {
      // Random starting row position
      drops[i] = (int)(Random::next() % (win.height / Graphics::CHARACTER_HEIGHT));
    }
    initialized = true;
  }

  void draw(Window &win) {
    if (!initialized) {
      init_drops(win);
    }

    win.clear(fill_color);

    int cols = win.width / Graphics::CHARACTER_WIDTH;
    if (cols > MAX_COLS) cols = MAX_COLS;
    int rows = win.height / Graphics::CHARACTER_HEIGHT;

    for (int i = 0; i < cols; i++) {
      char random_char = (char)((Random::next() % 94) + 33); // printable ASCII
      int x = i * Graphics::CHARACTER_WIDTH;
      int y = drops[i] * Graphics::CHARACTER_HEIGHT;

      if (y >= 0 && y < win.height) {
        win.draw_char(random_char, x, y, Colors::EMERALD);
      }

      // Advance drop
      drops[i]++;

      // Reset to top with a slight random chance so they don't all sync up
      if (drops[i] > rows && (Random::next() % 10) == 0) {
        drops[i] = 0;
      }
    }
  }

  void on_draw(Window &win) override { 
    draw(win); 
  }

  void on_key(Window &win, const KeyEvent &ev) override {}
};
