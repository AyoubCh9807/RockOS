#pragma once

#include "../core/timer.hpp"
#include "window.hpp"
#include "window_app.hpp"
#include "../data/colors.hpp"

class ClockApp : public IWindowApp {
public:
  const char *name() const override { return "Clock"; }

  void on_create(Window &win) override { draw(win); }

  void draw(Window &win) {
    win.clear(0x101018);

    char uptime[64];
    Timer::get_formatted_time_into(uptime, sizeof(uptime));

    win.draw_string(uptime, 10, 10, Colors::WHITE);
  }

  /* Called automatically once a second by WindowManager::redraw_all,
     no key input needed, this is what makes the clock actually
     live. */
  void on_draw(Window &win) override { draw(win); }
};
