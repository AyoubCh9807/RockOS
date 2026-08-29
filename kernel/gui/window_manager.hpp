#pragma once

#include "../core/kernel.hpp"
#include "../core/timer.hpp"
#include "../drivers/keyboard.hpp"
#include "../shared/key_event.hpp"
#include "window.hpp"
#include "window_app.hpp"

/* NOTE: render() below calls Graphics::put_pixel(x, y, color) as a
   stand-in for whatever your actual boot/graphics.hpp exposes. Paste
   that file over if the real API differs (a blit/rect-fill function
   would be much faster than one put_pixel call per pixel per frame,
   worth switching to once you confirm what's available). */
#include "../../boot/graphics.hpp"

constexpr int MAX_WINDOWS = 32;

class WindowManager {
private:
  Window *windows[MAX_WINDOWS]{};
  IWindowApp *apps[MAX_WINDOWS]{};
  int count = 0;

  Window *focused_window = nullptr;

  void raise_to_front(Window *win) {
    int max_z = 0;
    for (int i = 0; i < count; i++)
      if (windows[i]->z_order > max_z)
        max_z = windows[i]->z_order;
    win->z_order = max_z + 1;
  }

public:
  Window *create_window(IWindowApp *app, int x, int y, int width, int height) {
    if (count >= MAX_WINDOWS)
      return nullptr;

    Window *win = new Window(x, y, width, height, app->name());
    windows[count] = win;
    apps[count] = app;
    count++;

    raise_to_front(win);
    focus(win);

    app->on_create(*win);
    app->on_draw(*win);

    return win;
  }

  void destroy_window(Window *win) {
    for (int i = 0; i < count; i++) {
      if (windows[i] != win)
        continue;

      apps[i]->on_destroy(*win);

      if (focused_window == win)
        focused_window = nullptr;

      delete win;

      // Shift everything after this one down, keep the arrays dense.
      for (int j = i; j < count - 1; j++) {
        windows[j] = windows[j + 1];
        apps[j] = apps[j + 1];
      }
      count--;
      return;
    }
  }

  void focus(Window *win) {
    if (focused_window)
      focused_window->focused = false;
    focused_window = win;
    if (win)
      win->focused = true;
  }

  Window *get_focused() const { return focused_window; }

  IWindowApp *app_for(Window *win) const {
    for (int i = 0; i < count; i++)
      if (windows[i] == win)
        return apps[i];
    return nullptr;
  }

  void redraw(Window *win) {
    IWindowApp *app = app_for(win);
    if (app)
      app->on_draw(*win);
  }

  /* Composites every window onto the real screen, back to front by
     z_order. Simple insertion sort since MAX_WINDOWS is small, not
     worth anything fancier. */
  void render() {
    Window *ordered[MAX_WINDOWS];
    for (int i = 0; i < count; i++)
      ordered[i] = windows[i];

    for (int i = 1; i < count; i++) {
      Window *key = ordered[i];
      int j = i - 1;
      while (j >= 0 && ordered[j]->z_order > key->z_order) {
        ordered[j + 1] = ordered[j];
        j--;
      }
      ordered[j + 1] = key;
    }

    for (int i = 0; i < count; i++) {
      Window *win = ordered[i];
      for (int ly = 0; ly < win->height; ly++) {
        for (int lx = 0; lx < win->width; lx++) {
          u32 color = win->get_pixel(lx, ly);
          Graphics::put_pixel(win->x + lx, win->y + ly, color);
        }
      }
    }
  }

  void route_key(const KeyEvent &ev) {
    if (!focused_window)
      return;

    IWindowApp *app = app_for(focused_window);
    if (!app)
      return;

    app->on_key(*focused_window, ev);
    redraw(focused_window);
  }

  /* Redraws every window, not just the focused one. Apps that only
     ever change in response to their own key input don't need this,
     but anything that updates on its own (a clock, an animation)
     needs someone to call on_draw for it periodically, this is that
     someone. */
  void redraw_all() {
    for (int i = 0; i < count; i++)
      apps[i]->on_draw(*windows[i]);
  }

  /* Blocking loop, same shape as Shell::run(). Meant to be called
     instead of shell.run() while testing the GUI in isolation, later
     this and the shell will need to coexist (probably as two things
     the user can switch between, or the shell becomes one more
     window), not solved yet. */
  void run() {
    render();

    u32 last_refresh_tick = Timer::get_ticks();

    while (1) {
      KeyEvent ev = Keyboard::read();

      if (ev.scancode != 0 && ev.keytype != KeyType::None) {
        route_key(ev);
        render();
      }

      /* Once per second (100 ticks at the current 100Hz timer),
         redraw everything and recomposite, regardless of whether any
         key was pressed. This is what lets something like a clock
         update on its own. */
      u32 now = Timer::get_ticks();
      if (now - last_refresh_tick >= 100) {
        last_refresh_tick = now;
        redraw_all();
        render();
      }

      Kernel::halt();
    }
  }
};
