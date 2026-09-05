#pragma once

#include "../../boot/graphics.hpp"
#include "../data/colors.hpp"
#include "../shared/types.hpp"
#include "../utils/string_utils.hpp"

#include "icon_bitmaps.hpp"

class DesktopIcon {
private:
  const DesktopIconData *data;

  u32 x;
  u32 y;

  bool is_selected = false;

public:
  DesktopIcon(const char *label, u32 x, u32 y) : data(nullptr), x(x), y(y) {
    use_data_of(label);
  }

  DesktopIcon() : data(nullptr), x(0), y(0) {}

  void draw() {
    if (!data)
      return;

    int draw_x = (x >= 4) ? x - 4 : 0;
    int draw_y = (y >= 4) ? y - 4 : 0;

    if (selected()) {
      Graphics::draw_rect(draw_x, draw_y, data->width + 8, data->height + 8,
                          Colors::WHITE);
    }

    Graphics::draw_image(data->pixels, x, y, data->width, data->height);
    const char *label = data->label;

    u32 label_width = StringUtils::strlen(label) * Graphics::CHARACTER_WIDTH;

    u32 label_x = x + (data->width / 2) - (label_width / 2);
    u32 label_y = y + data->height + 4;

    Graphics::draw_string(label, label_x, label_y, Colors::WHITE);
  }

  // For future mouse events.
  bool contains(u32 mouse_x, u32 mouse_y) const {
    if (!data)
      return false;

    return mouse_x >= x && mouse_x < x + data->width && mouse_y >= y &&
           mouse_y < y + data->height;
  }

  const char *get_label() const { return data ? data->label : nullptr; }

  u32 get_x() const { return x; }

  u32 get_y() const { return y; }

  u32 get_width() const { return data ? data->width : 0; }

  u32 get_height() const { return data ? data->height : 0; }

  bool use_data_of(const char *label) {
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

  const u32 *get_pixels() const { return data ? data->pixels : nullptr; }

  void select() {
    is_selected = true;
  }

  void unselect() {
    is_selected = false;
  }

  bool selected() const { return is_selected; }

  const char *get_app() const { return data->label; }
};
