#pragma once

#include "../../core/rtc.hpp"
#include "../../data/colors.hpp"
#include "../../shared/types.hpp"
#include "../../utils/string_utils.hpp"
#include "../../../boot/graphics.hpp"
#include "iwidget.hpp"

namespace DateWidgetInternal {

static int text_width(const char *s) {
  int n = 0;
  while (s[n] != '\0')
    n++;
  return n * Graphics::CHARACTER_WIDTH;
}

static const char *MONTH_NAMES[] = {
    "",       "January", "February", "March",     "April",
    "May",    "June",    "July",     "August",    "September",
    "October", "November", "December"
};

// Zeller's congruence. Returns 0=Saturday .. 6=Friday.
static int weekday_index(int day, int month, int year) {
  int m = month;
  int y = year;

  if (m < 3) {
    m += 12;
    y -= 1;
  }

  const int k = y % 100;
  const int j = y / 100;

  const int h =
      (day + (13 * (m + 1)) / 5 + k + k / 4 + j / 4 + 5 * j) % 7;

  return h;
}

static const char *WEEKDAY_NAMES[] = {
    "SATURDAY", "SUNDAY",    "MONDAY",  "TUESDAY",
    "WEDNESDAY", "THURSDAY", "FRIDAY"
};

} // namespace DateWidgetInternal

class DateWidget : public IWidget {
public:
  DateWidget() {
    x = Multiboot2::framebuffer.width - 400;
    y = 220;

    width = 200;
    height = 140;

    min_width = 160;
    min_height = 110;
    max_width = 280;
    max_height = 200;

    z_index = 0;

    background_color = Colors::DARK_GRAY;
    border_color = Colors::AZURE;
    text_color = Colors::WHITE;

    padding = 8;
  }

  void update() override {}

  void draw() override {
    using namespace DateWidgetInternal;

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

    const int day = RTC::get_day();
    const int month = RTC::get_month();
    const int year = 2000 + RTC::get_year();

    const int weekday = weekday_index(day, month, year);

    // Header bar with weekday name
    constexpr int HEADER_HEIGHT = 28;

    Graphics::draw_rect(
        x + 2,
        y + 2,
        width - 4,
        HEADER_HEIGHT,
        border_color
    );

    const char *weekday_name = WEEKDAY_NAMES[weekday];

    Graphics::draw_string(
        weekday_name,
        x + (width - text_width(weekday_name)) / 2,
        y + 2 + (HEADER_HEIGHT - Graphics::CHARACTER_HEIGHT) / 2,
        Colors::BLACK
    );

    // Big day number
    char day_buf[4];
    StringUtils::snprintf(day_buf, sizeof(day_buf), "%d", day);

    Graphics::draw_string(
        day_buf,
        x + (width - text_width(day_buf)) / 2,
        y + HEADER_HEIGHT + 26,
        text_color
    );

    // Month name
    const char *month_name = MONTH_NAMES[month];

    Graphics::draw_string(
        month_name,
        x + (width - text_width(month_name)) / 2,
        y + HEADER_HEIGHT + 52,
        Colors::SILVER
    );

    // Year
    char year_buf[8];
    StringUtils::snprintf(year_buf, sizeof(year_buf), "%d", year);

    Graphics::draw_string(
        year_buf,
        x + (width - text_width(year_buf)) / 2,
        y + HEADER_HEIGHT + 70,
        Colors::SLATE
    );
  }
};
