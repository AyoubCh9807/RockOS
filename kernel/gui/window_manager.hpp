#pragma once

#include "../core/kernel.hpp"
#include "../core/timer.hpp"
#include "../drivers/keyboard.hpp"
#include "../shared/key_event.hpp"
#include "window.hpp"
#include "window_app.hpp"

#include "../../boot/graphics.hpp"

constexpr int WINDOW_UPDATE_TICKS = 5;
constexpr int FRAME_TICKS = 2; // arnd 50fps
constexpr int MAX_WINDOWS = 32;

constexpr u32 HEAP_SAFETY_MARGIN = 2 * 1024 * 1024; // 2 MiB

class WindowManager {
private:
  Window *windows[MAX_WINDOWS]{};
  IWindowApp *apps[MAX_WINDOWS]{};
  int count = 0;
  u32 last_refresh_tick = 0;

  Window *focused_window = nullptr;

  void raise_to_front(Window *win) {
    int max_z = 0;
    for (int i = 0; i < count; i++)
      if (windows[i]->z_order > max_z)
        max_z = windows[i]->z_order;
    win->z_order = max_z + 1;
  }

public:
  WindowManager() : last_refresh_tick(Timer::get_ticks()) {}

  Window *create_window(IWindowApp *app, int x, int y, int width, int height) {

    constexpr u32 SAFETY_MARGIN = 2 * 1024 * 1024;

    // Make room before allocating another ~1 MB window.
    while (heap.get_used() + SAFETY_MARGIN >= heap.get_size()) {
      if (count == 0)
        return nullptr;

      remove_oldest_window();
    }

    Window *win = new Window(x, y, width, height, app->name());

    if (!win)
      return nullptr;

    windows[count] = win;
    apps[count] = app;
    count++;

    raise_to_front(win);
    focus(win);

    app->on_create(*win);
    app->on_draw(*win);

    return win;
  }

  Window *get_window(int index) {
    if (index >= count || index < 0)
      return nullptr;
    return windows[index];
  }
  int get_count() { return count; }

  void destroy_window(Window *win) {
    for (int i = 0; i < count; i++) {
      if (windows[i] != win)
        continue;

      bool was_focused = (focused_window == win);
      int closed_z = win->z_order;

      apps[i]->on_destroy(*win);
      delete win;

      for (int j = i; j < count - 1; j++) {
        windows[j] = windows[j + 1];
        apps[j] = apps[j + 1];
      }

      count--;

      if (was_focused) {
        Window *previous = nullptr;
        int best_z = -1;

        for (int j = 0; j < count; j++) {
          if (windows[j]->z_order < closed_z && windows[j]->z_order > best_z) {
            previous = windows[j];
            best_z = windows[j]->z_order;
          }
        }

        // If there was no window underneath it, fall back to the
        // most top remaining window.
        if (!previous) {
          for (int j = 0; j < count; j++) {
            if (!previous || windows[j]->z_order > previous->z_order)
              previous = windows[j];
          }
        }

        focus(previous);
      }

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
    // Graphics::clear(Colors::BLACK);

    Window *ordered[MAX_WINDOWS];

    for (int i = 0; i < count; i++)
      ordered[i] = windows[i];

    // Sort back → front.
    for (int i = 1; i < count; i++) {
      Window *key = ordered[i];
      int j = i - 1;

      while (j >= 0 && ordered[j]->z_order > key->z_order) {
        ordered[j + 1] = ordered[j];
        j--;
      }

      ordered[j + 1] = key;
    }

    // Composite windows into the global back buffer.
    for (int i = 0; i < count; i++) {
      Window *win = ordered[i];

      for (int ly = 0; ly < win->height; ly++) {
        for (int lx = 0; lx < win->width; lx++) {
          u32 color = win->get_pixel(lx, ly);

          Graphics::put_pixel(win->x + lx, win->y + ly, color);
        }
      }
    }

    if (focused_window)
      draw_focus_border(focused_window);

    // Graphics::present(); Does not go here because its better if the desktop
    // owns the final frame presentation
  }

  void route_key(const KeyEvent &ev) {
    if (!focused_window)
      return;

    if (ev.scancode == (int)'x') {
      destroy_window(focused_window);
      return;
    }

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

  void update() {
    u32 now = Timer::get_ticks();

    if (now - last_refresh_tick >= WINDOW_UPDATE_TICKS) {
      last_refresh_tick = now;
      redraw_all();
    }
  }

  void remove_oldest_window() {
    if (count == 0)
      return;

    destroy_window(windows[0]);
  }

  void draw_focus_border(Window *win) {
    if (!win)
      return;

    constexpr int BORDER = 3;
    constexpr u32 COLOR = Colors::RED;

    // Top
    for (int x = 0; x < win->width; x++) {
      for (int i = 0; i < BORDER; i++)
        Graphics::put_pixel(win->x + x, win->y + i, COLOR);
    }

    // Bottom
    for (int x = 0; x < win->width; x++) {
      for (int i = 0; i < BORDER; i++)
        Graphics::put_pixel(win->x + x, win->y + win->height - 1 - i, COLOR);
    }

    // Left
    for (int y = 0; y < win->height; y++) {
      for (int i = 0; i < BORDER; i++)
        Graphics::put_pixel(win->x + i, win->y + y, COLOR);
    }

    // Right
    for (int y = 0; y < win->height; y++) {
      for (int i = 0; i < BORDER; i++)
        Graphics::put_pixel(win->x + win->width - 1 - i, win->y + y, COLOR);
    }
  }
};
