#pragma once

#include "../../boot/graphics.hpp"
#include "../containers/string.hpp"
#include "../data/colors.hpp"
#include "../shared/key_event.hpp"
#include "../drivers/mouse.hpp"

// Slot colors
static constexpr u32 SLOT_SHADOW_COLOR = Colors::BLACK;
static constexpr u32 SLOT_BORDER_COLOR = Colors::WHITE;
static constexpr u32 SLOT_BACKGROUND_COLOR = Colors::DARK_GRAY;
static constexpr u32 SLOT_TITLE_COLOR = Colors::RED;
static constexpr u32 SLOT_TEXT_COLOR = Colors::WHITE;
static constexpr u32 SLOT_BUTTON_COLOR = Colors::RED;
static constexpr u32 SLOT_BUTTON_TEXT_COLOR = Colors::WHITE;

class SelectionMenuOption {
private:
  int x;
  int y;
  int width;
  int height;

  String title;
  String description;

  bool is_hovered = false;
  bool is_selected = false;
  bool is_clicked = false;
public:
  SelectionMenuOption(int x, int y, int width, int height, const String &title,
         const String &desc)
      : x(x), y(y), width(width), height(height), title(title),
        description(desc) {}

  void draw() {

    

    constexpr int TITLE_BAR_HEIGHT = 30;
    constexpr int PADDING = 16;
    constexpr int BUTTON_HEIGHT = 28;
    constexpr int BUTTON_WIDTH = 80;

    // Shadow
    Graphics::draw_rect(
        x + 5, y + 5, width, height,
        SLOT_SHADOW_COLOR
    );

    // Outer dialog
    Graphics::draw_rect(
        x, y, width, height,
        SLOT_BORDER_COLOR
    );

    // Title bar
    Graphics::draw_rect(
        x + 2, y + 2,
        width - 4, TITLE_BAR_HEIGHT,
        SLOT_TITLE_COLOR
    );

    // Title
    Graphics::draw_string(
        title.c_str(),
        x + PADDING,
        y + 10,
        SLOT_TEXT_COLOR
    );

    // description will be handled l8r
    /* Graphics::draw_string(
        desc.c_str(),
        x + PADDING,
        y + TITLE_BAR_HEIGHT + 20,
        SLOT_TEXT_COLOR
    ); */  

  }

  bool handle_key(KeyEvent &ev) {
    if (ev.keytype == KeyType::Escape ||
        ev.keytype == KeyType::Enter) {
      return true;
    }

    return false;
  }

  constexpr bool hover() { is_hovered = true; }
  constexpr bool unhover() { is_hovered = false; }
  constexpr bool select() { is_selected = true; }
  constexpr bool unselect() { is_selected = false; }

  constexpr bool hovered() const { return is_hovered; }
  constexpr bool selected() const { return is_selected; }

  constexpr bool clicked() { return is_clicked; }

  constexpr bool is_currently_left_clicked() const { return Mouse::is_left_button_down() && hovered(); }
  constexpr bool is_currently_right_clicked() const { return Mouse::is_right_button_down() && hovered(); }
  constexpr bool is_currently_middle_clicked() const { return Mouse::is_middle_button_down() && hovered(); }
   
  constexpr bool contains(u32 mouse_x, u32 mouse_y) const {

    return mouse_x >= x && mouse_x < x + width && mouse_y >= y &&
           mouse_y < y + height;
  }

  void update() {
    is_hovered = contains(Mouse::get_x(), Mouse::get_y());
    // This only account when the option is clicked via the left mouse button only, right and middle mouse button are not handled
    is_clicked = is_currently_left_clicked();
  }

};

