#pragma once

// TODO: confirm the real include path for the global `heap` object.
// Guessed here as ../../memory/heap.hpp — adjust to wherever `heap`
// (with a get_used() method) is actually declared.
#include "../../memory/heap.hpp"

#include "../../../boot/graphics.hpp"
#include "../../data/colors.hpp"
#include "../../shared/types.hpp"
#include "../../utils/string_utils.hpp"
#include "iwidget.hpp"

namespace MemoryWidgetInternal {

static int text_width(const char *s) {
  int n = 0;
  while (s[n] != '\0')
    n++;
  return n * Graphics::CHARACTER_WIDTH;
}

} // namespace MemoryWidgetInternal

// Shows current heap usage and whether it is trending up, down, or
// holding steady since the last update. Deliberately avoids a
// percentage-of-total bar since no heap capacity API is known yet —
// swap in a proper fill bar once heap.get_total() (or similar)
// exists.
class MemoryWidget : public IWidget {
public:
  MemoryWidget() {
    x = Multiboot2::framebuffer.width - 620;
    y = 20;

    width = 220;
    height = 100;

    min_width = 180;
    min_height = 80;
    max_width = 320;
    max_height = 150;

    z_index = 0;

    background_color = Colors::DARK_GRAY;
    border_color = Colors::TURQUOISE;
    text_color = Colors::WHITE;

    padding = 8;

    last_used = 0;
    trend = 0;
    have_baseline = false;
  }

  void update() override {
    const u64 current_used = static_cast<u64>(heap.get_used());

    if (have_baseline) {
      if (current_used > last_used)
        trend = 1;
      else if (current_used < last_used)
        trend = -1;
      else
        trend = 0;
    }

    last_used = current_used;
    have_baseline = true;
  }

  void draw() override {
    using namespace MemoryWidgetInternal;

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
    Graphics::draw_string("MEMORY", x + padding, y + padding, border_color);

    // Trend glyph, drawn with plain lines so it doesn't depend on
    // any extra font glyphs
    const int glyph_cx = x + width - padding - 8;
    const int glyph_cy = y + padding + 4;

    u32 trend_color = Colors::SILVER;
    if (trend > 0)
      trend_color = Colors::LIGHT_RED;
    else if (trend < 0)
      trend_color = Colors::MINT;

    if (trend > 0) {
      // Up arrow
      Graphics::draw_line(glyph_cx - 5, glyph_cy + 4, glyph_cx, glyph_cy - 4,
                          trend_color);
      Graphics::draw_line(glyph_cx + 5, glyph_cy + 4, glyph_cx, glyph_cy - 4,
                          trend_color);
    } else if (trend < 0) {
      // Down arrow
      Graphics::draw_line(glyph_cx - 5, glyph_cy - 4, glyph_cx, glyph_cy + 4,
                          trend_color);
      Graphics::draw_line(glyph_cx + 5, glyph_cy - 4, glyph_cx, glyph_cy + 4,
                          trend_color);
    } else {
      // Steady dash
      Graphics::draw_line(glyph_cx - 5, glyph_cy, glyph_cx + 5, glyph_cy,
                          trend_color);
    }

    // Used amount, shown in KB for readability
    const u64 used_kb = last_used / 1024;

    char used_buf[32];
    StringUtils::snprintf(used_buf, sizeof(used_buf), "%d KB",
                          static_cast<int>(used_kb));

    Graphics::draw_string(used_buf, x + (width - text_width(used_buf)) / 2,
                          y + height / 2, text_color);

    // Small usage bar purely as a visual pulse, width is relative
    // to a soft 64 MB reference scale, clamped, until a real total
    // is available.
    constexpr u64 REFERENCE_SCALE = 64ull * 1024ull * 1024ull;

    const int bar_x = x + padding;
    const int bar_y = y + height - padding - 6;
    const int bar_max_w = width - padding * 2;

    Graphics::draw_rect(bar_x, bar_y, bar_max_w, 6, Colors::GRAY);

    u64 fill = (last_used * static_cast<u64>(bar_max_w)) / REFERENCE_SCALE;
    if (fill > static_cast<u64>(bar_max_w))
      fill = static_cast<u64>(bar_max_w);

    Graphics::draw_rect(bar_x, bar_y, static_cast<u32>(fill), 6, border_color);
  }

  void handle_key(const KeyEvent &ev) override {}
  void handle_mouse_event(const MouseEvent &ev) override {}

private:
  u64 last_used;
  int trend; // -1 shrinking, 0 steady, 1 growing
  bool have_baseline;
};
