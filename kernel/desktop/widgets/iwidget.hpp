#pragma once
#include "../../data/colors.hpp"
#include "../../shared/key_event.hpp"
#include "../../shared/mouse_types.hpp"
#include "../../shared/types.hpp"

class IWidget {
protected:
  int x = 0;
  int y = 0;

  int width = 80;
  int height = 80;

  int min_width = 80;
  int min_height = 80;

  int max_width = 480;
  int max_height = 480;

  bool visible = true;

  int z_index = 0;

  u32 background_color = Colors::RED;
  u32 border_color = Colors::GOLD;
  u32 text_color = Colors::WHITE;

  int padding = 0;

public:
  virtual ~IWidget() = default;

  virtual void draw() = 0;
  virtual void update() = 0;

  void set_position(int new_x, int new_y) {
    x = new_x;
    y = new_y;
  }

  constexpr int get_width() const { return width; }
  constexpr int get_height() const { return height; }

  virtual void handle_mouse_event(const MouseEvent &ev) = 0;
  virtual void handle_key(const KeyEvent &event) = 0;

  constexpr bool contains(int mouse_x, int mouse_y) const {
    return mouse_x >= x && mouse_x < x + width && mouse_y >= y &&
           mouse_y < y + height;
  }
};
