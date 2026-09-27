#pragma once

#include "../../core/rtc.hpp"
#include "../../data/colors.hpp"
#include "../../shared/types.hpp"
#include "../../../boot/graphics.hpp"
#include "iwidget.hpp"

class ClockWidget : public IWidget {
public:
  ClockWidget() {
    x = Multiboot2::framebuffer.width - 400;
    y = 20;

    width = 180;
    height = 180;

    background_color = Colors::DARK_GRAY;
    border_color = Colors::GOLD;
    text_color = Colors::WHITE;

    padding = 8;
  }

  void update() override {}

  void draw() override {
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

    // Clock face
    Graphics::draw_circle(
        center_x,
        center_y,
        radius,
        border_color
    );

    Graphics::draw_circle(
        center_x,
        center_y,
        radius - 2,
        border_color
    );

    // Hour markers
    for (int hour = 0; hour < 12; hour++) {
      const float angle =
          (hour * 2.0f * 3.14159265f / 12.0f) -
          (3.14159265f / 2.0f);

      const int outer_x =
          center_x +
          static_cast<int>(
              MathUtils::cos(angle) * radius
          );

      const int outer_y =
          center_y +
          static_cast<int>(
              MathUtils::sin(angle) * radius
          );

      const int inner_radius = radius - 6;

      const int inner_x =
          center_x +
          static_cast<int>(
              MathUtils::cos(angle) * inner_radius
          );

      const int inner_y =
          center_y +
          static_cast<int>(
              MathUtils::sin(angle) * inner_radius
          );

      Graphics::draw_line(
          inner_x,
          inner_y,
          outer_x,
          outer_y,
          text_color
      );
    }

    const int hours = RTC::get_hours() % 12;
    const int minutes = RTC::get_minutes();
    const int seconds = RTC::get_seconds();

    // Hour hand
    const float hour_angle =
        ((hours + minutes / 60.0f) *
         2.0f * 3.14159265f / 12.0f) -
        (3.14159265f / 2.0f);

    Graphics::draw_angled_line(
        center_x,
        center_y,
        radius * 50 / 100,
        hour_angle,
        text_color
    );

    // Minute hand
    const float minute_angle =
        (minutes *
         2.0f * 3.14159265f / 60.0f) -
        (3.14159265f / 2.0f);

    Graphics::draw_angled_line(
        center_x,
        center_y,
        radius * 72 / 100,
        minute_angle,
        text_color
    );

    // Second hand
    const float second_angle =
        (seconds *
         2.0f * 3.14159265f / 60.0f) -
        (3.14159265f / 2.0f);

    Graphics::draw_angled_line(
        center_x,
        center_y,
        radius * 82 / 100,
        second_angle,
        Colors::RED
    );

    // Center pivot
    Graphics::draw_circle(
        center_x,
        center_y,
        3,
        border_color
    );
  }
};
