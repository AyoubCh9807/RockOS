#pragma once

#include "../../core/timer.hpp"
#include "../../data/colors.hpp"
#include "../../shared/types.hpp"
#include "../../utils/math_utils.hpp"
#include "../../utils/string_utils.hpp"
#include "../../../boot/graphics.hpp"
#include "iwidget.hpp"

namespace CpuWidgetInternal {

static int text_width(const char *s) {
  int n = 0;
  while (s[n] != '\0')
    n++;
  return n * Graphics::CHARACTER_WIDTH;
}

// Draws an arc (in degrees, 0 = right, increasing clockwise since
// screen y grows downward) by stepping in half-degree increments and
// stamping filled dots, several radii deep for thickness.
static void draw_arc(int cx, int cy, int radius, int thickness,
                     float start_deg, float end_deg, u32 color) {
  const float step = 0.5f;

  for (float deg = start_deg; deg <= end_deg; deg += step) {
    const float rad = deg * (MathUtils::PI / 180.0);

    for (int t = 0; t < thickness; t++) {
      const int r = radius - t;

      const int px =
          cx + static_cast<int>(MathUtils::cos(rad) * r);
      const int py =
          cy + static_cast<int>(MathUtils::sin(rad) * r);

      Graphics::put_pixel(px, py, color);
    }
  }
}

} // namespace CpuWidgetInternal

class CpuWidget : public IWidget {
public:
  CpuWidget() {
    x = Multiboot2::framebuffer.width - 200;
    y = 20;

    width = 180;
    height = 180;

    min_width = 140;
    min_height = 140;
    max_width = 240;
    max_height = 240;

    z_index = 0;

    background_color = Colors::DARK_GRAY;
    border_color = Colors::GOLD;
    text_color = Colors::WHITE;

    padding = 8;
  }

  void update() override {}

  void draw() override {
    using namespace CpuWidgetInternal;

    if (!visible)
      return;

    // Widget background
    Graphics::draw_rect(
        x,
        y,
        width,
        height,
        background_color
    );

    // Widget border
    Graphics::draw_rect(
        x,
        y,
        width,
        2,
        border_color
    );

    Graphics::draw_rect(
        x,
        y + height - 2,
        width,
        2,
        border_color
    );

    Graphics::draw_rect(
        x,
        y,
        2,
        height,
        border_color
    );

    Graphics::draw_rect(
        x + width - 2,
        y,
        2,
        height,
        border_color
    );

    const int center_x = x + width / 2;
    const int center_y = y + height / 2;

    const int radius =
        (width < height ? width : height) / 2 - padding - 4;

    const int usage = MathUtils::clamp(0, Timer::get_cpu_usage(), 100);

    // Gauge sweeps 270 degrees with a 90 degree gap centered at the
    // bottom: from 135 deg (down-left) through the top to 45 deg
    // (down-right).
    constexpr float START_DEG = 135.0f;
    constexpr float SWEEP_DEG = 270.0f;

    // Background track (dim, full sweep)
    draw_arc(
        center_x,
        center_y,
        radius,
        4,
        START_DEG,
        START_DEG + SWEEP_DEG,
        Colors::SLATE
    );

    // Color reflects load: calm green, cautious gold, hot red
    u32 fill_color = Colors::LIME;
    if (usage >= 80)
      fill_color = Colors::RED;
    else if (usage >= 50)
      fill_color = Colors::GOLD;

    const float fill_deg = SWEEP_DEG * (usage / 100.0f);

    draw_arc(
        center_x,
        center_y,
        radius,
        4,
        START_DEG,
        START_DEG + fill_deg,
        fill_color
    );

    // Percentage text, centered
    char pct_buf[6];
    StringUtils::snprintf(pct_buf, sizeof(pct_buf), "%d%%", usage);

    Graphics::draw_string(
        pct_buf,
        center_x - text_width(pct_buf) / 2,
        center_y - Graphics::CHARACTER_HEIGHT / 2,
        text_color
    );

    // Label below the percentage
    Graphics::draw_string(
        "CPU",
        center_x - text_width("CPU") / 2,
        center_y + Graphics::CHARACTER_HEIGHT + 4,
        Colors::SILVER
    );
  }
};
