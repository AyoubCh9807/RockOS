#pragma once

#include "../../../boot/graphics.hpp"
#include "../../data/colors.hpp"
#include "../../shared/types.hpp"
#include "iwidget.hpp"

class ClickMeWidget : public IWidget {
private:
  u32 clicks = 0;

  static constexpr int TILE_SIZE = 16;

public:
  ClickMeWidget() {
    x = 20;
    y = 20;

    width = 200;
    height = 100;

    min_width = 160;
    min_height = 80;
    max_width = 320;
    max_height = 180;

    z_index = 1;

    background_color = Colors::DARK_GRAY;
    border_color = Colors::WHITE;
    text_color = Colors::WHITE;

    padding = 10;
  }

  void update() override {}

  void draw() override {
    if (!visible)
      return;

    // Chessboard background
    for (int py = y; py < y + height; py += TILE_SIZE) {
      for (int px = x; px < x + width; px += TILE_SIZE) {
        bool dark = ((px - x) / TILE_SIZE + (py - y) / TILE_SIZE) % 2;

        Graphics::draw_rect(
            px,
            py,
            MathUtils::min(TILE_SIZE, x + width - px),
            MathUtils::min(TILE_SIZE, y + height - py),
            dark ? background_color : Colors::RED);
      }
    }

    // Border
    Graphics::draw_rect(x, y, width, 2, border_color);
    Graphics::draw_rect(x, y + height - 2, width, 2, border_color);
    Graphics::draw_rect(x, y, 2, height, border_color);
    Graphics::draw_rect(x + width - 2, y, 2, height, border_color);

    // Text
    Graphics::draw_string(
        "CLICK ME:",
        x + padding,
        y + padding,
        text_color);

    char clicks_text[32];
    StringUtils::snprintf(
        clicks_text,
        sizeof(clicks_text),
        "CLICKS: %u",
        clicks);

    Graphics::draw_string(
        clicks_text,
        x + padding,
        y + padding + 24,
        text_color);
  }

  void click() {
    clicks++;
  }

  u32 get_clicks() const {
    return clicks;
  }

  void handle_mouse_event(const MouseEvent &ev) override {
    if(ev.event_type == MouseEventType::PRESS && ev.button_type == MouseButton::LEFT_BUTTON) {
      click();
    }
  }
  void handle_key(const KeyEvent &event) override {}
};
