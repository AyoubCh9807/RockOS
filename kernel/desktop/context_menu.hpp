#pragma once

#include "../containers/string.hpp"
#include "../data/colors.hpp"
#include "../drivers/mouse.hpp"
#include "../shared/mouse_types.hpp"
#include "../shared/types.hpp"
#include "context_menu_actions.hpp"
#include "desktop_actions.hpp"

class ContextMenu {
public:
  struct Style {
    u32 background_color;
    u32 border_color;
    u32 text_color;
    u32 disabled_text_color;
    u32 hover_color;
    u32 separator_color;

    u32 border_width;
    u32 padding;
    u32 item_height;
  };

private:
  struct Item {
    String label;
    Action action = nullptr;
    void *context = nullptr;

    bool separator = false;
    bool enabled = true;
  };

  static constexpr u32 MAX_ITEMS = 16;
  static constexpr u32 SEPARATOR_HEIGHT = 8;
  static constexpr int INVALID_ITEM = -1;

  Item items[MAX_ITEMS];
  u32 item_count = 0;

  int x = 0;
  int y = 0;

  u32 width = 180;

  int hovered_item = INVALID_ITEM;

  bool visible = false;
  bool close_after_action = true;

  Style style;

  DesktopActions &actions;

public:
  ContextMenu(DesktopActions &actions) : actions(actions) {
    style.background_color = Colors::DARK_GRAY;
    style.border_color = Colors::WHITE;
    style.text_color = Colors::WHITE;
    style.disabled_text_color = Colors::GRAY;
    style.hover_color = Colors::DARK_RED;
    style.separator_color = Colors::GRAY;

    style.border_width = 2;
    style.padding = 8;
    style.item_height = 28;
  }
  void open(int x, int y) {
    const int screen_width = Multiboot2::framebuffer.width;
    const int screen_height = Multiboot2::framebuffer.height;

    const int menu_width = static_cast<int>(width);
    const int menu_height = static_cast<int>(get_height());

    if (x + menu_width > screen_width)
      x = screen_width - menu_width;

    if (y + menu_height > screen_height)
      y = screen_height - menu_height;

    if (x < 0)
      x = 0;

    if (y < 0)
      y = 0;

    set_position(x, y);
    hovered_item = INVALID_ITEM;
    visible = true;
  }
  void close() { visible = false; };

  void clear() {
    item_count = 0;
    hovered_item = INVALID_ITEM;
    visible = false;
  };

  bool add_item(const char *label, Action action, void *context = nullptr) {
    if (!label || !action)
      return false;

    if (item_count >= MAX_ITEMS)
      return false;

    Item &item = items[item_count];

    item.label = label;
    item.action = action;
    item.context = context;
    item.separator = false;
    item.enabled = true;

    ++item_count;

    return true;
  }

  bool add_item(const char *label, ContextMenuActions action) {
    return add_item(label, get_action(action), &actions);
  }

  bool add_separator() {
    if (item_count >= MAX_ITEMS)
      return false;

    Item &item = items[item_count];

    item.label = "";
    item.action = nullptr;
    item.context = nullptr;
    item.separator = true;
    item.enabled = false;

    ++item_count;

    return true;
  }
  void set_position(int x, int y) {
    this->x = x;
    this->y = y;
  }

  void set_width(u32 width) { this->width = width; }

  void set_item_height(u32 height) { style.item_height = height; }

  void set_style(const Style &style) { this->style = style; }

  void set_close_after_action(bool close) { close_after_action = close; }

  int get_item_at(int mouse_x, int mouse_y) const {
    if (!contains(mouse_x, mouse_y))
      return INVALID_ITEM;

    int current_y = y + style.padding;

    for (u32 i = 0; i < item_count; ++i) {
      const int height =
          items[i].separator ? SEPARATOR_HEIGHT : style.item_height;

      if (mouse_y >= current_y && mouse_y < current_y + height) {

        if (items[i].separator)
          return INVALID_ITEM;

        return static_cast<int>(i);
      }

      current_y += height;
    }

    return INVALID_ITEM;
  }

  u32 get_height() const {
    u32 height = style.padding * 2;

    for (u32 i = 0; i < item_count; ++i) {
      height += items[i].separator ? SEPARATOR_HEIGHT : style.item_height;
    }

    return height;
  }

  bool contains(int mouse_x, int mouse_y) const {
    return mouse_x >= x && mouse_x < x + static_cast<int>(width) &&
           mouse_y >= y && mouse_y < y + static_cast<int>(get_height());
  }

  void update() {
    if (!visible)
      return;

    hovered_item = get_item_at(Mouse::get_x(), Mouse::get_y());
  }

  void draw() {
    if (!visible)
      return;

    const u32 height = get_height();

    // Background
    Graphics::draw_rect(x, y, width, height, style.background_color);

    // Border
    for (u32 i = 0; i < style.border_width; ++i) {
      Graphics::draw_line(x + i, y + i, width - i * 2, style.border_color);

      Graphics::draw_line(x + i, y + height - 1 - i, width - i * 2,
                          style.border_color);

      Graphics::draw_vertical_line(x + i, y + i, height - i * 2,
                                   style.border_color);

      Graphics::draw_vertical_line(x + width - 1 - i, y + i, height - i * 2,
                                   style.border_color);
    }

    int current_y = y + style.padding;

    for (u32 i = 0; i < item_count; ++i) {
      Item &item = items[i];

      if (item.separator) {
        const int separator_y = current_y + SEPARATOR_HEIGHT / 2;

        Graphics::draw_line(x + style.padding, separator_y,
                            width - style.padding * 2, style.separator_color);

        current_y += SEPARATOR_HEIGHT;
        continue;
      }

      // Hover background
      if (static_cast<int>(i) == hovered_item) {
        Graphics::draw_rect(x + style.border_width, current_y,
                            width - style.border_width * 2, style.item_height,
                            style.hover_color);
      }

      const u32 text_color =
          item.enabled ? style.text_color : style.disabled_text_color;

      Graphics::draw_string(
          item.label.c_str(), x + style.padding,
          current_y + (style.item_height - Graphics::CHARACTER_HEIGHT) / 2,
          text_color);

      current_y += style.item_height;
    }
  }
  bool handle_mouse(const MouseEvent &ev) {
    if (!visible)
      return false;

    hovered_item = get_item_at(Mouse::get_x(), Mouse::get_y());

    if (ev.event_type == MouseEventType::PRESS &&
        ev.button_type == MouseButton::LEFT_BUTTON) {

      if (hovered_item != INVALID_ITEM) {
        Item &item = items[hovered_item];

        if (item.enabled && item.action) {
          item.action(item.context);

          if (close_after_action)
            close();
        }

        return true;
      }

      // Clicked outside the menu.
      close();
      return true;
    }

    return true;
  }

  constexpr bool is_visible() const { return visible; }

  constexpr int get_hovered_item() const { return hovered_item; }
};
