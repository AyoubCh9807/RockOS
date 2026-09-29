#pragma once

#include "../../memory/heap.hpp"

#include "../../../boot/graphics.hpp"
#include "../../core/timer.hpp"
#include "../../data/colors.hpp"
#include "../../shared/types.hpp"
#include "../../utils/math_utils.hpp"
#include "../../utils/string_utils.hpp"
#include "iwidget.hpp"

namespace SystemInfoWidgetInternal {

static int text_width(const char *s) {
  int n = 0;
  while (s[n] != '\0')
    n++;
  return n * Graphics::CHARACTER_WIDTH;
}

static void draw_mini_bar(int x, int y, int w, int h, int percent,
                          u32 track_color, u32 fill_color) {
  Graphics::draw_rect(x, y, w, h, track_color);

  int clamped = MathUtils::clamp(0, percent, 100);
  int fill_w = (w * clamped) / 100;

  Graphics::draw_rect(x, y, fill_w, h, fill_color);
}

} // namespace SystemInfoWidgetInternal

// A compact dashboard card summarizing the whole system at a glance:
// OS name, uptime, CPU load, and heap usage, each with a small
// proportional bar. Meant to sit alongside the more detailed
// single-purpose widgets, not replace them.
class SystemInfoWidget : public IWidget {
public:
  SystemInfoWidget() {
    x = 20;
    y = 20;

    width = 240;
    height = 170;

    min_width = 200;
    min_height = 140;
    max_width = 320;
    max_height = 240;

    // Drawn above the other cards since it's meant as the primary
    // at-a-glance summary
    z_index = 1;

    background_color = Colors::DARK_GRAY;
    border_color = Colors::LAVENDER;
    text_color = Colors::WHITE;

    padding = 10;
  }

  void update() override {}

  void draw() override {
    using namespace SystemInfoWidgetInternal;

    if (!visible)
      return;

    // Widget background
    Graphics::draw_rect(x, y, width, height, background_color);

    // Widget border
    Graphics::draw_rect(x, y, width, 2, border_color);

    Graphics::draw_rect(x, y + height - 2, width, 2, border_color);

    Graphics::draw_rect(x, y, 2, height, border_color);

    Graphics::draw_rect(x + width - 2, y, 2, height, border_color);

    int cursor_y = y + padding;

    // Title
    Graphics::draw_string("ROCKOS", x + padding, cursor_y, border_color);

    cursor_y += 20;

    Graphics::draw_line(x + padding, cursor_y, x + width - padding, cursor_y,
                        Colors::SLATE);

    cursor_y += 12;

    // Uptime row
    char uptime_buf[32];
    Timer::get_formatted_time_into(uptime_buf, sizeof(uptime_buf));

    Graphics::draw_string("UPTIME", x + padding, cursor_y, Colors::SILVER);

    const char *uptime_digits = uptime_buf;
    while (*uptime_digits != '\0' &&
           !(*uptime_digits >= '0' && *uptime_digits <= '9'))
      uptime_digits++;

    Graphics::draw_string(uptime_digits,
                          x + width - padding - text_width(uptime_digits),
                          cursor_y, text_color);

    cursor_y += 22;

    // CPU row
    const int cpu_usage = MathUtils::clamp(0, Timer::get_cpu_usage(), 100);

    char cpu_label[16];
    StringUtils::snprintf(cpu_label, sizeof(cpu_label), "CPU %d%%", cpu_usage);

    Graphics::draw_string(cpu_label, x + padding, cursor_y, Colors::SILVER);

    cursor_y += 12;

    u32 cpu_color = Colors::LIME;
    if (cpu_usage >= 80)
      cpu_color = Colors::RED;
    else if (cpu_usage >= 50)
      cpu_color = Colors::GOLD;

    draw_mini_bar(x + padding, cursor_y, width - padding * 2, 6, cpu_usage,
                  Colors::GRAY, cpu_color);

    cursor_y += 22;

    // Memory row (approximate, see memory_widget.hpp TODO about a
    // real capacity API)
    const u64 used_kb = static_cast<u64>(heap.get_used()) / 1024;

    char mem_label[24];
    StringUtils::snprintf(mem_label, sizeof(mem_label), "MEM %d KB",
                          static_cast<int>(used_kb));

    Graphics::draw_string(mem_label, x + padding, cursor_y, Colors::SILVER);

    cursor_y += 12;

    constexpr u64 REFERENCE_SCALE_KB = 64ull * 1024ull;
    int mem_percent = static_cast<int>((used_kb * 100ull) / REFERENCE_SCALE_KB);

    draw_mini_bar(x + padding, cursor_y, width - padding * 2, 6, mem_percent,
                  Colors::GRAY, Colors::TURQUOISE);
  }

  void handle_key(const KeyEvent &ev) override {}
  void handle_mouse_event(const MouseEvent &ev) override {}
};
