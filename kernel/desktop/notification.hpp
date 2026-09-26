#pragma once

#include "../../boot/graphics.hpp"
#include "../../boot/multiboot2.hpp"
#include "../containers/string.hpp"
#include "../core/timer.hpp"
#include "../data/colors.hpp"
#include "../utils/text_utils.hpp"
#include "notification_icons.hpp"

class Notification {

  static constexpr u32 DEFAULT_WIDTH = 200;
  static constexpr u32 DEFAULT_HEIGHT = 120;

  static constexpr u32 DEFAULT_MARGIN = 16;
  static constexpr u32 DEFAULT_PADDING = 12;

  static constexpr u32 DEFAULT_TITLE_Y = 10;
  static constexpr u32 DEFAULT_MESSAGE_Y = 36;

  static constexpr u32 DEFAULT_ICON_SIZE = 32;
  static constexpr u32 DEFAULT_ICON_MARGIN = 10;

  static constexpr u32 DEFAULT_BORDER_THICKNESS = 0;

  static constexpr u32 CHARACTER_WIDTH = 8;

private:
  String title;
  String description;
  String message;

  u32 timeout = 0;
  const u32 *icon = nullptr;

  bool is_visible = false;
  bool is_expired = false;

  u32 width = DEFAULT_WIDTH;
  u32 height = DEFAULT_HEIGHT;

  u32 x = 0;
  u32 y = 0;

  u32 margin = DEFAULT_MARGIN;
  u32 padding = DEFAULT_PADDING;

  u32 background_color = Colors::LIGHT_RED;
  u32 border_color = Colors::BLACK;

  u32 title_color = Colors::BLACK;
  u32 description_color = Colors::LIGHT_GRAY;

  u32 border_thickness = DEFAULT_BORDER_THICKNESS;

  u32 icon_size = DEFAULT_ICON_SIZE;
  u32 icon_margin = DEFAULT_ICON_MARGIN;

  u32 title_y = DEFAULT_TITLE_Y;
  u32 message_y = DEFAULT_MESSAGE_Y;

  u32 shown_at = 0;

  bool auto_position = true;
  bool auto_wrap = true;

public:
  Notification(const String &title, const String &description, u32 timeout,
               const u32 *icon)
      : title(title), description(description), timeout(timeout), icon(icon) {
    wrap_message();
  }

  Notification(const String &title, const String &description, u32 timeout)
      : title(title), description(description), timeout(timeout),
        icon(ROCK_OS_ICON_BATTERY_CHARGING) {
    wrap_message();
  }

  Notification() = default;
  ~Notification() = default;

  void draw() const {
    if (!is_visible)
      return;

    u32 draw_x = x;
    u32 draw_y = y;

    if (auto_position) {
      const u32 screen_width = Multiboot2::framebuffer.width;

      if (width + margin <= screen_width)
        draw_x = screen_width - width - margin;
      else
        draw_x = 0;

      draw_y = margin;
    }

    if (border_thickness > 0 && border_thickness * 2 < width &&
        border_thickness * 2 < height) {

      Graphics::draw_rect(draw_x, draw_y, width, height, border_color);

      Graphics::draw_rect(draw_x + border_thickness, draw_y + border_thickness,
                          width - border_thickness * 2,
                          height - border_thickness * 2, background_color);

    } else {
      Graphics::draw_rect(draw_x, draw_y, width, height, background_color);
    }

    if (icon) {
      Graphics::draw_image(icon, draw_x + width - icon_size - icon_margin,
                           draw_y + icon_margin, icon_size, icon_size);
    }

    Graphics::draw_string(title.c_str(), draw_x + padding, draw_y + title_y,
                          title_color);

    Graphics::draw_string(message.c_str(), draw_x + padding, draw_y + message_y,
                          description_color);
  }

  void update() {
    if (!is_visible || is_expired)
      return;

    const u32 now = Timer::get_ticks();

    if (now - shown_at >= timeout) {
      is_visible = false;
      is_expired = true;
    }
  }

  void show() {
    shown_at = Timer::get_ticks();
    is_visible = true;
    is_expired = false;
  }

  void hide() { is_visible = false; }

  void expire() {
    is_expired = true;
    is_visible = false;
  }

  bool visible() const { return is_visible; }

  bool expired() const { return is_expired; }

  void set_width(u32 value) {
    width = value;
    wrap_message();
  }

  void set_height(u32 value) { height = value; }

  void set_position(u32 value_x, u32 value_y) {
    x = value_x;
    y = value_y;
    auto_position = false;
  }

  void reset_position() { auto_position = true; }

  void set_margin(u32 value) { margin = value; }

  void set_padding(u32 value) {
    padding = value;
    wrap_message();
  }

  void set_background_color(u32 color) { background_color = color; }

  void set_border_color(u32 color) { border_color = color; }

  void set_border_thickness(u32 thickness) { border_thickness = thickness; }

  void set_title_color(u32 color) { title_color = color; }

  void set_description_color(u32 color) { description_color = color; }

  void set_icon(const u32 *value) { icon = value; }

  void set_icon_size(u32 value) { icon_size = value; }

  void set_icon_margin(u32 value) { icon_margin = value; }

  void set_title_y(u32 value) { title_y = value; }

  void set_message_y(u32 value) { message_y = value; }

  void set_timeout(u32 value) { timeout = value; }

  void set_title(const String &value) { title = value; }

  void set_description(const String &value) {
    description = value;
    wrap_message();
  }

  void set_auto_wrap(bool enabled) {
    auto_wrap = enabled;

    if (auto_wrap)
      wrap_message();
    else
      message = description;
  }

  u32 get_width() const { return width; }

  u32 get_height() const { return height; }

  u32 get_padding() const { return padding; }

  bool is_auto_wrap_enabled() const { return auto_wrap; }

private:
  void wrap_message() {
    if (!auto_wrap) {
      message = description;
      return;
    }

    const u32 text_width = width > padding * 2 ? width - padding * 2 : 0;

    message = TextUtils::wrap_text(description, text_width, CHARACTER_WIDTH);
  }
};
