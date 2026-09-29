#pragma once

#include "../../../boot/graphics.hpp"
#include "../../core/timer.hpp"
#include "../../data/colors.hpp"
#include "../../shared/types.hpp"
#include "iwidget.hpp"

namespace UptimeWidgetInternal {

static int text_width(const char *s) {
  int n = 0;
  while (s[n] != '\0')
    n++;
  return n * Graphics::CHARACTER_WIDTH;
}

} // namespace UptimeWidgetInternal

class UptimeWidget : public IWidget {
public:
  UptimeWidget() {
    x = Multiboot2::framebuffer.width - 400;
    y = 380;

    width = 200;
    height = 90;

    min_width = 160;
    min_height = 70;
    max_width = 280;
    max_height = 130;

    z_index = 0;

    background_color = Colors::DARK_GRAY;
    border_color = Colors::EMERALD;
    text_color = Colors::WHITE;

    padding = 8;
  }

  void update() override {}

  void draw() override {
    using namespace UptimeWidgetInternal;

    if (!visible)
      return;

    // Widget background
    Graphics::draw_rect(x, y, width, height, background_color);

    // Widget border
    Graphics::draw_rect(x, y, width, 2, border_color);

    Graphics::draw_rect(x, y + height - 2, width, 2, border_color);

    Graphics::draw_rect(x, y, 2, height, border_color);

    Graphics::draw_rect(x + width - 2, y, 2, height, border_color);

    // Label
    Graphics::draw_string("UPTIME", x + padding, y + padding, border_color);

    // Formatted uptime string, e.g. "Uptime: 01:23:45"
    char buf[32];
    Timer::get_formatted_time_into(buf, sizeof(buf));

    // Skip the "Uptime: " prefix since we already have a label above
    const char *time_str = buf;
    while (*time_str != '\0' && *time_str != ':' &&
           (*time_str < '0' || *time_str > '9'))
      time_str++;

    // Walk back to the start of the first digit group
    const char *cursor = buf;
    const char *digits_start = buf;
    while (*cursor != '\0') {
      if (*cursor >= '0' && *cursor <= '9') {
        digits_start = cursor;
        break;
      }
      cursor++;
    }

    Graphics::draw_string(digits_start,
                          x + (width - text_width(digits_start)) / 2,
                          y + padding + 22, text_color);

    // Activity indicator row: a dot lights up per second, cycling
    constexpr int DOT_COUNT = 10;
    constexpr int DOT_SPACING = 14;
    constexpr int DOT_RADIUS = 3;

    const int active_dot = Timer::get_seconds() % DOT_COUNT;

    const int row_width = DOT_COUNT * DOT_SPACING;
    const int start_x = x + (width - row_width) / 2 + DOT_SPACING / 2;
    const int dots_y = y + height - padding - DOT_RADIUS;

    for (int i = 0; i < DOT_COUNT; i++) {
      const u32 dot_color = (i == active_dot) ? border_color : Colors::SLATE;

      Graphics::draw_circle(start_x + i * DOT_SPACING, dots_y, DOT_RADIUS,
                            dot_color);
    }
  }

  void handle_key(const KeyEvent &ev) override {}
  void handle_mouse_event(const MouseEvent &ev) override {}
};
