#pragma once

#include "../../boot/graphics.hpp"
#include "../data/colors.hpp"
#include "../drivers/mouse.hpp"
#include "../shared/types.hpp"
#include "../utils/string_utils.hpp"

#include "icon_bitmaps.hpp"

class DesktopIcon {
private:
  const DesktopIconData *data;

  u32 x;
  u32 y;

  bool is_selected = false;
  bool is_hovered = false;

  bool is_left_clicked = false;
  bool is_right_clicked = false;
  bool is_middle_clicked = false;

  bool is_launchable = false;

  // The offset of the hover/highlight effects for the icon
  static constexpr auto ICON_OFFSET = 4;

public:
  DesktopIcon(const char *label, u32 x, u32 y) : data(nullptr), x(x), y(y) {
    use_data_of(label);
  }

  DesktopIcon() : data(nullptr), x(0), y(0) {}

  constexpr void draw() {
    if (!data)
      return;

    int draw_x = (x >= ICON_OFFSET) ? x - ICON_OFFSET : 0;
    int draw_y = (y >= ICON_OFFSET) ? y - ICON_OFFSET : 0;

    if (selected()) {
      Graphics::draw_rect(draw_x, draw_y, data->width + 8, data->height + 8,
                          Colors::WHITE);
    }

    else if (hovered()) {
      Graphics::draw_rect(draw_x, draw_y, data->width + 8, data->height + 8,
                          Colors::GRAY);
    }

    Graphics::draw_image(data->pixels, x, y, data->width, data->height);
    const char *label = data->label;

    u32 label_width = StringUtils::strlen(label) * Graphics::CHARACTER_WIDTH;

    u32 label_x = x + (data->width / 2) - (label_width / 2);
    u32 label_y = y + data->height + 4;

    Graphics::draw_string(label, label_x, label_y, Colors::WHITE);
  }

  // For mouse events
  constexpr bool contains(u32 mouse_x, u32 mouse_y) const {
    if (!data)
      return false;

    return mouse_x >= x && mouse_x < x + data->width && mouse_y >= y &&
           mouse_y < y + data->height;
  }

  const char *get_label() const { return data ? data->label : nullptr; }

  constexpr u32 get_x() const { return x; }

  constexpr u32 get_y() const { return y; }

  constexpr u32 get_width() const { return data ? data->width : 0; }

  constexpr u32 get_height() const { return data ? data->height : 0; }

  constexpr bool use_data_of(const char *label) {
    if (!label)
      return false;

    for (u32 i = 0; i < IconBitmaps::COUNT; i++) {
      const DesktopIconData *icon = IconBitmaps::ALL[i];

      if (StringUtils::strcmp(icon->label, label) == 0) {
        data = icon;
        return true;
      }
    }

    return false;
  }

  constexpr const u32 *get_pixels() const {
    return data ? data->pixels : nullptr;
  }

  constexpr void select() { is_selected = true; }

  constexpr void unselect() { is_selected = false; }

  constexpr void hover() { is_hovered = true; }
  constexpr void unhover() { is_hovered = false; }

  constexpr bool hovered() const { return is_hovered; }

  constexpr bool selected() const { return is_selected; }

  constexpr const char *get_app() const { return data->label; }

  constexpr void update() {
    if (contains(Mouse::get_x(), Mouse::get_y()))
      hover();
    else
      unhover();

    is_left_clicked = (hovered() && Mouse::is_left_button_down());
  }

  constexpr const bool currently_left_clicked() const {
    return is_left_clicked;
  }
  constexpr const bool currently_right_clicked() const {
    return is_right_clicked;
  }
  constexpr const bool currently_middle_clicked() const {
    return is_middle_clicked;
  }

  constexpr void make_launchable() { is_launchable = true; }
  constexpr void make_unlaunchable() { is_launchable = true; }
  constexpr bool launchable() { return is_launchable; }
};
