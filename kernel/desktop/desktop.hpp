#pragma once

#include "../../boot/graphics.hpp"

#include "../gui/window_app_registry.hpp"
#include "../gui/window_manager.hpp"
#include "desktop_icon.hpp"

#include "../gui/counter_app.hpp"
#include "../gui/dice_app.hpp"
#include "../gui/dvd_app.hpp"
#include "../gui/matrix_app.hpp"
#include "../gui/paint_app.hpp"
#include "../gui/tyrant_app.hpp"

static constexpr auto MAX_DESKTOP_APPS = 256;
static constexpr auto DEFAULT_WINDOW_WIDTH = 600;
static constexpr auto DEFAULT_WINDOW_HEIGHT = 480;

class Desktop {
private:
  WindowManager &window_manager;
  WindowAppRegistry &window_app_registry;

  DesktopIcon icons[MAX_DESKTOP_APPS];
  int icon_count = 0;
  int selected_icon = 0;
  
  u32 background_color = Colors::DARK_RED;

  void draw_background() { Graphics::clear(background_color); }

  int LAUNCH_X = 0;
  int LAUNCH_Y = 0;
  int LAUNCH_DX = 32;
  int LAUNCH_DY = 32;

  static constexpr auto SCREEN_WIDTH = 640;
  static constexpr auto SCREEN_HEIGHT = 480;


  void handle_launch_coords() {
    if(LAUNCH_X >= SCREEN_WIDTH) LAUNCH_DX = -LAUNCH_DX;
    if(LAUNCH_Y >= SCREEN_HEIGHT) LAUNCH_DY = -LAUNCH_DY;

    if(LAUNCH_X <= 0) LAUNCH_DX = -LAUNCH_DX;
    if(LAUNCH_Y <= 0) LAUNCH_DY = -LAUNCH_DY;

    LAUNCH_X += LAUNCH_DX;
    LAUNCH_DY += LAUNCH_DY;
  }

public:
  Desktop(WindowManager &wm, WindowAppRegistry &window_app_registry)
      : window_manager(wm), window_app_registry(window_app_registry) {}

  void init() { clear(); }

  void update() {
    KeyEvent ev = Keyboard::read();

    if (ev.scancode != 0 && ev.keytype != KeyType::None) {
      if (handle_key(ev))
        return;

      window_manager.route_key(ev);
    }

    window_manager.update();
  }

  void render() {
    draw_background();

    window_manager.render();

    //    draw_wallpaper();
    draw_icons();

    //    draw_taskbar();
  }

  void clear() { draw_background(); }

  //  void draw_taskbar();

  void draw_icons() {
    for (int i = 0; i < icon_count; i++) {
      if (selected_icon == i)
        icons[i].select();
      else
        icons[i].unselect();

      icons[i].draw();
    }
  }

  // void draw_wallpaper();

  void add_icon(const char *label, u32 x, u32 y) {
    if (icon_count >= MAX_DESKTOP_APPS)
      return;

    icons[icon_count++] = DesktopIcon(label, x, y);
  }

  void launch_app(DesktopIcon &icon) {
    IWindowApp *app = window_app_registry.find(icon.get_label());
    if (!app)
      return;

    window_manager.create_window(app, LAUNCH_X, LAUNCH_Y, DEFAULT_WINDOW_WIDTH,
                                 DEFAULT_WINDOW_HEIGHT);
  

    handle_launch_coords();

  }

  void select_next_icon() { selected_icon = (selected_icon + 1) % icon_count; }
  void select_prev_icon() {
    if (selected_icon == 0) {
      selected_icon = icon_count - 1;
      return;
    };
    selected_icon--;
  }

  bool handle_key(const KeyEvent &ev) {
    if (ev.keytype == KeyType::ArrowRight) {
      select_next_icon();
      return true;
    }
    if (ev.keytype == KeyType::ArrowLeft) {
      select_prev_icon();
      return true;
    }
    if (ev.keytype == KeyType::Enter || (ev.keytype == KeyType::Char && ev.scancode == 'o')) {
      launch_app(icons[selected_icon]);
      return true;
    }
    return false;
  }

  void run() {
    u32 last_frame_tick = Timer::get_ticks();

    while (1) {
      update();

      u32 now = Timer::get_ticks();

      if (now - last_frame_tick >= FRAME_TICKS) {
        last_frame_tick = now;
        render();
      }

      Kernel::halt();
    }
  }
};
