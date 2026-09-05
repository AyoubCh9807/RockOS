#include "../window.hpp"
#include "../window_app.hpp"

class AboutApp : public IWindowApp {
private:
  u32 fill_color = Colors::DARK_GRAY;
  u32 text_color = Colors::WHITE;

public:
  const char *name() const override {
    return "About";
  }

  void on_create(Window &win) override {
    draw(win);
  }

  void draw(Window &win) {
    win.clear(fill_color);

    win.draw_string("ROCK OS", 50, 50, Colors::RED);
    win.draw_string("About Rock OS", 50, 80, text_color);

    win.draw_string("Version 0.1.0", 50, 120, text_color);
    win.draw_string("A tiny operating system... for now...", 50, 150, text_color);
    win.draw_string("built from scratch with C++.", 50, 170, text_color);

    win.draw_string("Made with Rock.", 50, 220, Colors::GOLD);
  }

  void on_draw(Window &win) override {
    draw(win);
  }

  void on_key(Window &win, const KeyEvent &ev) override {
    // Nothing for now
  }
};
