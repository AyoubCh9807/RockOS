#pragma once

#include "../core/kernel.hpp"
#include "../core/timer.hpp"
#include "../drivers/keyboard.hpp"
#include "../drivers/mouse.hpp"
#include "../shared/key_event.hpp"
#include "window.hpp"
#include "window_app.hpp"

#include "../../boot/graphics.hpp"

constexpr int WINDOW_UPDATE_TICKS = 5;
constexpr int FRAME_TICKS = 2;
constexpr int MAX_WINDOWS = 32;

constexpr u32 HEAP_SAFETY_MARGIN = 2 * 1024 * 1024;

constexpr int WINDOW_BORDER_THICKNESS = 2;
constexpr int WINDOW_TITLE_BAR_HEIGHT = 24;
constexpr int WINDOW_BUTTON_SIZE = 18;
constexpr int WINDOW_BUTTON_GAP = 2;
constexpr int WINDOW_BUTTON_MARGIN = 3;

constexpr int WINDOW_CHAR_WIDTH = 8;
constexpr int WINDOW_CHAR_HEIGHT = 8;

class WindowManager {
private:
  Window *windows[MAX_WINDOWS]{};
  IWindowApp *apps[MAX_WINDOWS]{};
  int count = 0;
  u32 last_refresh_tick = 0;

  Window *focused_window = nullptr;

  void raise_to_front(Window *win) {
    int max_z = 0;

    for (int i = 0; i < count; i++) {
      if (windows[i]->z_order > max_z)
        max_z = windows[i]->z_order;
    }

    win->z_order = max_z + 1;
  }

  void draw_rect(int x, int y, int width, int height, u32 color) {
    for (int py = y; py < y + height; py++) {
      for (int px = x; px < x + width; px++)
        Graphics::put_pixel(px, py, color);
    }
  }

  void draw_char(char c, int x, int y, u32 color) {
    if ((u8)c >= 128)
      return;

    const u8 *glyph = Graphics::FONT[(u8)c];

    for (int row = 0; row < WINDOW_CHAR_HEIGHT; row++) {
      const u8 bits = glyph[row];

      for (int col = 0; col < WINDOW_CHAR_WIDTH; col++) {
        if (bits & (1 << (WINDOW_CHAR_WIDTH - 1 - col)))
          Graphics::put_pixel(x + col, y + row, color);
      }
    }
  }

  void draw_string(const char *str, int x, int y, u32 color) {
    if (!str)
      return;

    int i = 0;

    while (str[i] != '\0') {
      draw_char(str[i], x + i * WINDOW_CHAR_WIDTH, y, color);
      i++;
    }
  }

  void draw_border(Window *win, int thickness, u32 color) {
    if (!win || win->minimized)
      return;

    const int x = win->x;
    const int y = win->y;
    const int width = win->width;
    const int height = win->height;

    draw_rect(x, y, width, thickness, color);
    draw_rect(x, y + height - thickness, width, thickness, color);

    draw_rect(x, y, thickness, height, color);
    draw_rect(x + width - thickness, y, thickness, height, color);
  }

  void draw_title_bar(Window *win) {
    if (!win || win->minimized)
      return;

    const int x = win->x;
    const int y = win->y;

    const u32 bar_color = win->focused ? Colors::DARK_RED : Colors::DARK_GRAY;

    draw_rect(x, y, win->width, WINDOW_TITLE_BAR_HEIGHT, bar_color);

    if (win->title) {
      draw_string(win->title, x + 8,
                  y + (WINDOW_TITLE_BAR_HEIGHT - WINDOW_CHAR_HEIGHT) / 2,
                  Colors::WHITE);
    }

    const int close_x =
        x + win->width - WINDOW_BUTTON_MARGIN - WINDOW_BUTTON_SIZE;

    const int maximize_x = close_x - WINDOW_BUTTON_GAP - WINDOW_BUTTON_SIZE;

    const int minimize_x = maximize_x - WINDOW_BUTTON_GAP - WINDOW_BUTTON_SIZE;

    const int button_y = y + (WINDOW_TITLE_BAR_HEIGHT - WINDOW_BUTTON_SIZE) / 2;

    draw_rect(minimize_x, button_y, WINDOW_BUTTON_SIZE, WINDOW_BUTTON_SIZE,
              Colors::GRAY);

    draw_rect(maximize_x, button_y, WINDOW_BUTTON_SIZE, WINDOW_BUTTON_SIZE,
              Colors::GRAY);

    draw_rect(close_x, button_y, WINDOW_BUTTON_SIZE, WINDOW_BUTTON_SIZE,
              Colors::RED);

    draw_string("_", minimize_x + 5, button_y + 3, Colors::WHITE);

    draw_string("[]", maximize_x + 1, button_y + 4, Colors::WHITE);

    draw_string("X", close_x + 5, button_y + 4, Colors::WHITE);
  }

  Window::Button title_bar_button(Window *win, int local_x, int local_y) {
    if (!win || win->minimized)
      return Window::Button::NONE;

    if (local_y < 0 || local_y >= WINDOW_TITLE_BAR_HEIGHT)
      return Window::Button::NONE;

    const int close_x = win->width - WINDOW_BUTTON_MARGIN - WINDOW_BUTTON_SIZE;

    const int maximize_x = close_x - WINDOW_BUTTON_GAP - WINDOW_BUTTON_SIZE;

    const int minimize_x = maximize_x - WINDOW_BUTTON_GAP - WINDOW_BUTTON_SIZE;

    const int button_y = (WINDOW_TITLE_BAR_HEIGHT - WINDOW_BUTTON_SIZE) / 2;

    if (local_y < button_y || local_y >= button_y + WINDOW_BUTTON_SIZE)
      return Window::Button::NONE;

    if (local_x >= minimize_x && local_x < minimize_x + WINDOW_BUTTON_SIZE)
      return Window::Button::MINIMIZE;

    if (local_x >= maximize_x && local_x < maximize_x + WINDOW_BUTTON_SIZE)
      return Window::Button::MAXIMIZE;

    if (local_x >= close_x && local_x < close_x + WINDOW_BUTTON_SIZE)
      return Window::Button::CLOSE;

    return Window::Button::NONE;
  }

  void draw_chrome(Window *win) {
    if (!win || win->minimized)
      return;

    draw_title_bar(win);

    draw_border(win, WINDOW_BORDER_THICKNESS, Colors::WHITE);
  }

  void handle_window_button(Window *win, Window::Button button) {
    if (!win)
      return;

    switch (button) {
    case Window::Button::CLOSE:
      destroy_window(win);
      break;

    case Window::Button::MAXIMIZE:
      // Real resizing can be implemented here later.
      break;

    case Window::Button::MINIMIZE:
      win->minimize();
      break;

    case Window::Button::NONE:
      break;
    }
  }

public:
  WindowManager() : last_refresh_tick(Timer::get_ticks()) {}

  Window *create_window(IWindowApp *app, int x, int y, int width, int height) {

    while (true) {
      if (heap.get_used() + HEAP_SAFETY_MARGIN < heap.get_size()) {
        Window *win = new Window(x, y, width, height, app->name());

        if (win) {
          windows[count] = win;
          apps[count] = app;
          count++;

          raise_to_front(win);
          focus(win);

          app->on_create(*win);

          if (!win->minimized)
            app->on_draw(*win);

          return win;
        }
      }

      if (count == 0)
        return nullptr;

      remove_oldest_window();
    }
  }

  Window *get_window(int index) {
    if (index < 0 || index >= count)
      return nullptr;

    return windows[index];
  }

  int get_count() { return count; }

  void destroy_window(Window *win) {
    for (int i = 0; i < count; i++) {
      if (windows[i] != win)
        continue;

      const bool was_focused = focused_window == win;
      const int closed_z = win->z_order;

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
          if (windows[j]->minimized)
            continue;

          if (windows[j]->z_order < closed_z && windows[j]->z_order > best_z) {
            previous = windows[j];
            best_z = windows[j]->z_order;
          }
        }

        if (!previous) {
          for (int j = 0; j < count; j++) {
            if (windows[j]->minimized)
              continue;

            if (!previous || windows[j]->z_order > previous->z_order) {
              previous = windows[j];
            }
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
    for (int i = 0; i < count; i++) {
      if (windows[i] == win)
        return apps[i];
    }

    return nullptr;
  }

  void redraw(Window *win) {
    IWindowApp *app = app_for(win);

    if (app)
      app->on_draw(*win);
  }

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

      if (win->minimized)
        continue;

      for (int ly = 0; ly < win->height; ly++) {
        for (int lx = 0; lx < win->width; lx++) {
          const u32 color = win->get_pixel(lx, ly);

          Graphics::put_pixel(win->x + lx, win->y + ly, color);
        }
      }

      draw_title_bar(win);

      draw_border(win, WINDOW_BORDER_THICKNESS, Colors::WHITE);
    }

    if (focused_window && !focused_window->minimized)
      draw_focus_border(focused_window);
  }

  void destroy_all_windows() {
    while (count > 0)
      destroy_window(windows[0]);
  }

  void route_key(const KeyEvent &ev) {
    if (!focused_window)
      return;

    if (ev.scancode == (int)'x') {
      destroy_window(focused_window);
      return;
    }

    if (ev.scancode == 'k' && Keyboard::is_ctrl_down()) {
      destroy_all_windows();
      return;
    }

    IWindowApp *app = app_for(focused_window);

    if (!app)
      return;

    app->on_key(*focused_window, ev);
    redraw(focused_window);
  }

  void route_mouse_event(const MouseEvent &ev) {
    const bool is_left_click = ev.button_type == MouseButton::LEFT_BUTTON &&
                               ev.event_type == MouseEventType::PRESS;

    if (is_left_click) {
      Window *clicked_window = nullptr;

      int highest_z = -1;

      for (int i = 0; i < count; i++) {
        Window *win = windows[i];

        if (win->minimized)
          continue;

        if (!win->contains(Mouse::get_x(), Mouse::get_y()))
          continue;

        if (win->z_order > highest_z) {
          highest_z = win->z_order;
          clicked_window = win;
        }
      }

      if (clicked_window) {
        focus(clicked_window);
        raise_to_front(clicked_window);
        redraw_all();

        const int local_x = Mouse::get_x() - clicked_window->x;

        const int local_y = Mouse::get_y() - clicked_window->y;

        const Window::Button button =
            title_bar_button(clicked_window, local_x, local_y);

        if (button != Window::Button::NONE) {
          handle_window_button(clicked_window, button);
          return;
        }
      }
    }

    if (!focused_window || focused_window->minimized)
      return;

    IWindowApp *app = app_for(focused_window);

    if (!app)
      return;

    app->on_mouse_event(*focused_window, ev);
    redraw(focused_window);
  }

  void redraw_all() {
    for (int i = 0; i < count; i++) {
      if (windows[i]->minimized)
        continue;

      apps[i]->on_draw(*windows[i]);
    }
  }

  void update() {
    const u32 now = Timer::get_ticks();

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
    if (!win || win->minimized)
      return;

    constexpr int BORDER = 3;
    constexpr u32 COLOR = Colors::RED;

    for (int x = 0; x < win->width; x++) {
      for (int i = 0; i < BORDER; i++)
        Graphics::put_pixel(win->x + x, win->y + i, COLOR);
    }

    for (int x = 0; x < win->width; x++) {
      for (int i = 0; i < BORDER; i++)
        Graphics::put_pixel(win->x + x, win->y + win->height - 1 - i, COLOR);
    }

    for (int y = 0; y < win->height; y++) {
      for (int i = 0; i < BORDER; i++)
        Graphics::put_pixel(win->x + i, win->y + y, COLOR);
    }

    for (int y = 0; y < win->height; y++) {
      for (int i = 0; i < BORDER; i++)
        Graphics::put_pixel(win->x + win->width - 1 - i, win->y + y, COLOR);
    }
  }

  constexpr bool any_window_contains(int x, int y) {
    for (int i = 0; i < count; i++) {
      if (windows[i]->minimized)
        continue;

      if (windows[i]->contains(x, y))
        return true;
    }

    return false;
  }
};
