#pragma once

#include "../../boot/graphics.hpp"
#include "../data/colors.hpp"
#include "../drivers/mouse.hpp"
#include "../shared/key_event.hpp"
#include "desktop.hpp"
#include "icon_bitmaps.hpp"

#include "../gui/dialog_manager.hpp"
#include "../gui/window_app_registry.hpp"
#include "../gui/window_manager.hpp"

constexpr auto MAX_QUERY_LENGTH = 64;

class AppLauncher {

private:
  WindowManager &window_manager;
  WindowAppRegistry &window_app_registry;
  DialogManager &dialog_manager;

  bool open_state = false;
  u32 selected_app = 0;
  u32 visible_start = 0; // Track scroll position as a member variable

  static constexpr u32 MAX_VISIBLE = 10;

  char query[MAX_QUERY_LENGTH] = "";
  size_t query_length = 0;

  const DesktopIconData *filtered_apps[IconBitmaps::COUNT];
  u32 filtered_count = 0;

public:
  AppLauncher(WindowManager &wm, WindowAppRegistry &reg, DialogManager &dm)
      : window_manager(wm), window_app_registry(reg), dialog_manager(dm) {}

  void open() {
    open_state = true;
    query[0] = '\0';
    query_length = 0;
    selected_app = 0;
    visible_start = 0;

    update_filtered_apps();
  }

  void close() { open_state = false; }
  void toggle() { open_state = !open_state; };

  void select_prev_app() {
    if (filtered_count == 0)
      return;

    if (selected_app > 0) {
      selected_app--;
      // Scroll up only if we go past the top of the visible window
      if (selected_app < visible_start) {
        visible_start = selected_app;
      }
    }
  }

  void select_next_app() {
    if (filtered_count == 0)
      return;

    if (selected_app + 1 < filtered_count) {
      selected_app++;
      // Scroll down only if we go past the bottom of the visible window
      if (selected_app >= visible_start + MAX_VISIBLE) {
        visible_start = selected_app - MAX_VISIBLE + 1;
      }
    }
  }

  void draw() {
    if (!open_state)
      return;

    constexpr u32 X = 210;
    constexpr u32 Y = 120;
    constexpr u32 W = 500;
    constexpr u32 H = 400;

    constexpr u32 BORDER = 2;
    constexpr u32 SEARCH_H = 38;
    constexpr u32 ROW_H = 30;

    const u32 right = X + W;
    const u32 bottom = Y + H;

    Graphics::draw_rect(X + 8, Y + 8, W, H, 0x70000000);

    Graphics::draw_rect(X, Y, W, H, 0xFF120909);

    Graphics::draw_rect(X, Y, W, BORDER, Colors::CORAL);
    Graphics::draw_rect(X, bottom - BORDER, W, BORDER, Colors::CORAL);
    Graphics::draw_rect(X, Y, BORDER, H, Colors::CORAL);
    Graphics::draw_rect(right - BORDER, Y, BORDER, H, Colors::CORAL);

    Graphics::draw_string(">", X + 18, Y + 14, Colors::CORAL);

    if (query[0] == '\0') {
      Graphics::draw_string("Find application...", X + 34, Y + 14,
                            Colors::GRAY);
    } else {
      Graphics::draw_string(query, X + 34, Y + 14, Colors::WHITE);
    }

    Graphics::draw_rect(X + 16, Y + SEARCH_H - 4, W - 32, 1, Colors::GRAY);

    char count_text[16];

    StringUtils::format(count_text, "%d results", filtered_count);

    Graphics::draw_string(count_text, right - 90, Y + 15, Colors::GRAY);

    constexpr u32 RESULTS_X = X + 14;
    constexpr u32 RESULTS_Y = Y + SEARCH_H + 10;

    u32 visible_end = visible_start + MAX_VISIBLE;

    if (visible_end > filtered_count)
      visible_end = filtered_count;

    for (u32 i = visible_start; i < visible_end; ++i) {
      const DesktopIconData *app = filtered_apps[i];

      if (!app)
        continue;

      const u32 row = i - visible_start;
      const u32 row_y = RESULTS_Y + row * ROW_H;

      const bool selected = i == selected_app;

      if (selected) {
        Graphics::draw_rect(RESULTS_X, row_y, W - 28, ROW_H, 0xFF351010);

        Graphics::draw_rect(RESULTS_X, row_y, 3, ROW_H, Colors::CORAL);
      }

      const u32 icon_x = RESULTS_X + 14;
      const u32 icon_y = row_y + 7;

      for (u32 py = 0; py < app->height; ++py) {
        for (u32 px = 0; px < app->width; ++px) {
          const u32 pixel = app->pixels[py * app->width + px];

          if ((pixel >> 24) == 0)
            continue;

          Graphics::put_pixel(icon_x + px, icon_y + py, pixel);
        }
      }

      Graphics::draw_string(app->label, RESULTS_X + 42, row_y + 8,
                            selected ? Colors::WHITE : Colors::GRAY);

      if (selected) {
        Graphics::draw_string("ENTER", right - 76, row_y + 8, Colors::CORAL);
      }
    }

    if (filtered_count == 0) {
      Graphics::draw_string("No applications found", X + 28, RESULTS_Y + 42,
                            Colors::GRAY);

      Graphics::draw_string("Try another search", X + 28, RESULTS_Y + 64,
                            Colors::GRAY);
    }

    const u32 footer_y = bottom - 32;

    Graphics::draw_rect(X + 16, footer_y, W - 32, 1, Colors::GRAY);

    Graphics::draw_string("UP/DOWN", X + 22, footer_y + 10, Colors::GRAY);

    Graphics::draw_string("navigate", X + 82, footer_y + 10, Colors::GRAY);

    Graphics::draw_string("ENTER", X + 154, footer_y + 10, Colors::WHITE);

    Graphics::draw_string("open", X + 200, footer_y + 10, Colors::GRAY);

    Graphics::draw_string("ESC", right - 72, footer_y + 10, Colors::CORAL);

    Graphics::draw_string("close", right - 42, footer_y + 10, Colors::GRAY);
  }

  void handle_key(const KeyEvent &event) {
    if (event.keytype == KeyType::Enter) {
      open_selected_app();

    } else if (event.keytype == KeyType::Escape || event.scancode == 'x') {
      close();

    } else if (event.keytype == KeyType::ArrowDown) {
      select_next_app();

    } else if (event.keytype == KeyType::ArrowUp) {
      select_prev_app();

    } else if (event.keytype == KeyType::Char) {
      if (query_length + 1 >= MAX_QUERY_LENGTH)
        return;

      char c[2] = {(char)event.scancode, '\0'};

      StringUtils::append(query, c);
      query_length++;

      selected_app = 0;
      visible_start = 0;
      update_filtered_apps();

    } else if (event.keytype == KeyType::BackSpace) {
      if (query_length == 0)
        return;

      query[query_length - 1] = '\0';
      query_length--;

      selected_app = 0;
      visible_start = 0;
      update_filtered_apps();
    }
  }

  void handle_mouse(const MouseEvent &event) { return; };

  void update_filtered_apps() {
    filtered_count = 0;

    for (u32 i = 0; i < IconBitmaps::COUNT; ++i) {
      const DesktopIconData *app = IconBitmaps::ALL[i];

      if (!app)
        continue;

      if (query[0] == '\0') {
        filtered_apps[filtered_count++] = app;
        continue;
      }

      if (StringUtils::contains_ignore_case(app->label, query)) {
        filtered_apps[filtered_count++] = app;
      }
    }

    if (selected_app >= filtered_count)
      selected_app = filtered_count > 0 ? filtered_count - 1 : 0;

    if (selected_app < visible_start) {
      visible_start = selected_app;
    }
    if (visible_start + MAX_VISIBLE > filtered_count) {
      if (filtered_count >= MAX_VISIBLE) {
        visible_start = filtered_count - MAX_VISIBLE;
      } else {
        visible_start = 0;
      }
    }
  }

  bool is_open() const { return open_state; };

  void open_selected_app() {
    if (filtered_count == 0)
      return;

    const DesktopIconData *app = filtered_apps[selected_app];

    if (!app)
      return;

    IWindowApp *window_app = window_app_registry.find(app->label);

    if (!window_app)
      return;

    window_manager.create_window(window_app, 0, 0, 640, 480);

    close();
  }
};
