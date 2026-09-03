#pragma once

#include "../../boot/graphics.hpp"

#include "../data/system.hpp"
#include "../gui/window_manager.hpp"

class Desktop {
private:
  WindowManager &window_manager;

  u32 background_color = Colors::BLUE;

public:
  Desktop(WindowManager &wm) : window_manager(wm) {}

  void init() { clear(); }

  void update() {
    // Desktop logic
  }

  void render() {
    draw_background();

    // draw_taskbar();
    // draw_icons();
    // draw_wallpaper();

    window_manager.render();
  }

  void clear() { draw_background(); }

private:
  void draw_background() {
    Graphics::clear(background_color);
  }
};
