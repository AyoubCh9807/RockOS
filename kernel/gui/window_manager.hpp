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

static constexpr int TASKBAR_HEIGHT = 64;

// Edge snapping
static constexpr int SNAP_THRESHOLD = 24;

// Double-click detection on the title bar.
// Timer::get_ticks() runs at TIMER_HZ (100 Hz), so 40 ticks = 400 ms.
static constexpr u32 TITLEBAR_DOUBLE_CLICK_TICKS = 40;

class WindowManager {
private:
  Window *windows[MAX_WINDOWS]{};
  IWindowApp *apps[MAX_WINDOWS]{};
  int count = 0;
  u32 last_refresh_tick = 0;

  Window *focused_window = nullptr;

  bool dragging = false;
  Window *dragged_window = nullptr;

  int drag_offset_x = 0;
  int drag_offset_y = 0;

  // Double-click state
  Window *last_titlebar_click_window = nullptr;
  u32 last_titlebar_click_tick = 0;

  // Z-order management
  // Normalizes z values to 0..count-1 so they never grow without bound.
  void compact_z_order() {
    for (int z = 0; z < count; z++) {
      Window *next = nullptr;

      for (int i = 0; i < count; i++) {
        if (windows[i]->z_order != z)
          continue;

        next = windows[i];
        break;
      }

      if (!next) {
        int best = 0x7fffffff;

        for (int i = 0; i < count; i++) {
          if (windows[i]->z_order > z && windows[i]->z_order < best)
            best = windows[i]->z_order;
        }

        for (int i = 0; i < count; i++) {
          if (windows[i]->z_order == best) {
            windows[i]->z_order = z;
            break;
          }
        }
      }
    }
  }

  void raise_to_front(Window *win) {
    compact_z_order();

    int max_z = -1;

    for (int i = 0; i < count; i++) {
      if (windows[i]->z_order > max_z)
        max_z = windows[i]->z_order;
    }

    win->z_order = max_z + 1;
  }

  Window *window_at(int x, int y) {
    Window *best = nullptr;
    int best_z = -1;

    for (int i = 0; i < count; i++) {
      Window *win = windows[i];

      if (win->minimized)
        continue;

      if (!win->contains(x, y))
        continue;

      if (win->z_order > best_z) {
        best_z = win->z_order;
        best = win;
      }
    }

    return best;
  }

  // Primitives

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

  bool title_bar_contains(Window *win, int local_x, int local_y) {
    if (!win || win->minimized)
      return false;

    if (local_x < 0 || local_x >= win->width)
      return false;

    if (local_y < 0 || local_y >= WINDOW_TITLE_BAR_HEIGHT)
      return false;

    return true;
  }

  bool client_area_contains(Window *win, int local_x, int local_y) {
    if (!win || win->minimized)
      return false;

    if (local_x < WINDOW_BORDER_THICKNESS ||
        local_x >= win->width - WINDOW_BORDER_THICKNESS)
      return false;

    const int client_top = WINDOW_TITLE_BAR_HEIGHT + WINDOW_BORDER_THICKNESS;

    if (local_y < client_top ||
        local_y >= win->height - WINDOW_BORDER_THICKNESS)
      return false;

    return true;
  }

  void handle_window_button(Window *win, Window::Button button) {
    if (!win)
      return;

    switch (button) {
    case Window::Button::CLOSE:
      destroy_window(win);
      break;

    case Window::Button::MAXIMIZE:
      toggle_maximize(win);
      break;

    case Window::Button::MINIMIZE:
      win->minimize();

      // Focus falls through to the next visible window.
      if (focused_window == win)
        focus(topmost_visible_window());
      break;

    case Window::Button::NONE:
      break;
    }
  }

  void toggle_maximize(Window *win) {
    if (win->is_maximized())
      win->restore();
    else
      win->maximize(Multiboot2::framebuffer.width,
                    Multiboot2::framebuffer.height, TASKBAR_HEIGHT);

    redraw(win);
  }

  Window *topmost_visible_window() {
    Window *best = nullptr;
    int best_z = -1;

    for (int i = 0; i < count; i++) {
      if (windows[i]->minimized)
        continue;

      if (windows[i]->z_order > best_z) {
        best_z = windows[i]->z_order;
        best = windows[i];
      }
    }

    return best;
  }

  void clamp_window_position(Window *win) {
    if (!win)
      return;

    const int screen_width = Multiboot2::framebuffer.width;
    const int screen_height = Multiboot2::framebuffer.height;

    const int max_x = screen_width - win->width;
    const int max_y = screen_height - TASKBAR_HEIGHT - win->height;

    if (win->x < 0)
      win->x = 0;

    if (win->y < 0)
      win->y = 0;

    if (win->x > max_x)
      win->x = max_x;

    if (win->y > max_y)
      win->y = max_y;
  }

  // Magnetic edge snapping while dragging.
  void apply_snap(Window *win) {
    if (!win)
      return;

    const int screen_width = Multiboot2::framebuffer.width;
    const int screen_height = Multiboot2::framebuffer.height;
    const int max_y = screen_height - TASKBAR_HEIGHT - win->height;

    if (win->x < SNAP_THRESHOLD)
      win->x = 0;
    else if (screen_width - (win->x + win->width) < SNAP_THRESHOLD)
      win->x = screen_width - win->width;

    if (win->y < SNAP_THRESHOLD)
      win->y = 0;
    else if (max_y - win->y < SNAP_THRESHOLD)
      win->y = max_y;
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

public:
  WindowManager() : last_refresh_tick(Timer::get_ticks()) {}

  Window *create_window(IWindowApp *app, int client_x, int client_y,
                        int client_width, int client_height) {
    // Calculate total window size including borders and title bar
    const int total_width = client_width + (2 * WINDOW_BORDER_THICKNESS);
    const int total_height =
        client_height + WINDOW_TITLE_BAR_HEIGHT + (2 * WINDOW_BORDER_THICKNESS);

    // Adjust starting position so the *client* area starts where requested
    const int win_x = client_x - WINDOW_BORDER_THICKNESS;
    const int win_y =
        client_y - WINDOW_TITLE_BAR_HEIGHT - WINDOW_BORDER_THICKNESS;

    while (true) {
      if (count < MAX_WINDOWS &&
          heap.get_used() + HEAP_SAFETY_MARGIN < heap.get_size()) {
        Window *win =
            new Window(win_x, win_y, total_width, total_height, app->name());

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

      if (dragged_window == win) {
        dragging = false;
        dragged_window = nullptr;
      }

      if (last_titlebar_click_window == win)
        last_titlebar_click_window = nullptr;

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

        if (!previous)
          previous = topmost_visible_window();

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

  void destroy_all_windows() {
    while (count > 0)
      destroy_window(windows[0]);
  }

  void render() {
    Window *ordered[MAX_WINDOWS];

    for (int i = 0; i < count; i++)
      ordered[i] = windows[i];

    // Insertion sort by z_order, back to front.
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

      const int client_offset_x = WINDOW_BORDER_THICKNESS;
      const int client_offset_y =
          WINDOW_TITLE_BAR_HEIGHT + WINDOW_BORDER_THICKNESS;

      for (int ly = 0;
           ly < win->height - client_offset_y - WINDOW_BORDER_THICKNESS; ly++) {
        for (int lx = 0; lx < win->width - (2 * WINDOW_BORDER_THICKNESS);
             lx++) {
          const u32 color = win->get_pixel(lx, ly);
          Graphics::put_pixel(win->x + client_offset_x + lx,
                              win->y + client_offset_y + ly, color);
        }
      }

      draw_title_bar(win);
      draw_border(win, WINDOW_BORDER_THICKNESS, Colors::WHITE);
    }

    if (focused_window && !focused_window->minimized)
      draw_focus_border(focused_window);
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

    // Ctrl+ArrowUp / Ctrl+ArrowDown to maximize/restore.
    if (Keyboard::is_ctrl_down() &&
        (ev.keytype == KeyType::ArrowUp || ev.keytype == KeyType::ArrowDown)) {
      toggle_maximize(focused_window);
      return;
    }

    IWindowApp *app = app_for(focused_window);

    if (!app)
      return;

    app->on_key(*focused_window, ev);
    redraw(focused_window);
  }

  void route_mouse_event(const MouseEvent &ev) {
    const int mouse_x = Mouse::get_x();
    const int mouse_y = Mouse::get_y();

    const bool is_left_press = ev.button_type == MouseButton::LEFT_BUTTON &&
                               ev.event_type == MouseEventType::PRESS;

    const bool is_left_release = ev.button_type == MouseButton::LEFT_BUTTON &&
                                 ev.event_type == MouseEventType::RELEASE;

    const bool is_move = ev.event_type == MouseEventType::MOVE;

    if (is_left_press) {
      Window *clicked_window = window_at(mouse_x, mouse_y);

      if (clicked_window) {
        if (focused_window != clicked_window) {
          focus(clicked_window);
          raise_to_front(clicked_window);
          redraw_all();
        }

        const int local_x = mouse_x - clicked_window->x;
        const int local_y = mouse_y - clicked_window->y;

        const Window::Button button =
            title_bar_button(clicked_window, local_x, local_y);

        if (button != Window::Button::NONE) {
          handle_window_button(clicked_window, button);
          return;
        }

        if (title_bar_contains(clicked_window, local_x, local_y)) {
          // Double-click on the title bar toggles maximize.
          // Timer ticks at TIMER_HZ (100 Hz) so: 40 ticks = 400 ms.
          const int now = ev.tick; // this was originally: (u32)Timer::get_ticks()

          if (last_titlebar_click_window == clicked_window &&
              now - last_titlebar_click_tick <=
                  (int)TITLEBAR_DOUBLE_CLICK_TICKS) {
            last_titlebar_click_window = nullptr;
            toggle_maximize(clicked_window);
            return;
          }

          last_titlebar_click_window = clicked_window;
          last_titlebar_click_tick = now;

          if (!clicked_window->is_maximized()) {
            dragging = true;
            dragged_window = clicked_window;

            drag_offset_x = local_x;
            drag_offset_y = local_y;
          }

          return;
        }

        // Clicks on the border / chrome are swallowed (no app event).
        if (!client_area_contains(clicked_window, local_x, local_y))
          return;

        IWindowApp *app = app_for(clicked_window);

        if (app) {
          app->on_mouse_event(*clicked_window, ev);
          redraw(clicked_window);
        }

        return;
      }

      // Clicked desktop: drop focus.
      focus(nullptr);
      return;
    }

    if (is_left_release) {
      dragging = false;
      dragged_window = nullptr;
    }

    if (dragging && dragged_window && is_move) {
      Window *win = dragged_window;

      win->x = mouse_x - drag_offset_x;
      win->y = mouse_y - drag_offset_y;

      clamp_window_position(win);
      apply_snap(win);

      redraw_all();
      return;
    }

    // Deliver move/release to the window under the cursor.
    Window *hovered = window_at(mouse_x, mouse_y);

    if (!hovered)
      return;

    const int hovered_local_x = mouse_x - hovered->x;
    const int hovered_local_y = mouse_y - hovered->y;

    // Mirror the PRESS path: chrome (title bar / border) never reaches
    // the app. Without this, ending a title-bar drag with a RELEASE
    // (or hovering the title bar during a MOVE) gets forwarded to the
    // app as if it happened inside the client area.
    if (!client_area_contains(hovered, hovered_local_x, hovered_local_y))
      return;

    IWindowApp *app = app_for(hovered);

    if (!app)
      return;

    app->on_mouse_event(*hovered, ev);
    redraw(hovered);
  }

  bool any_window_contains(int x, int y) { return window_at(x, y) != nullptr; }
};
