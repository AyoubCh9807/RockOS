#pragma once

#include "../../boot/graphics.hpp"
#include "../containers/string.hpp"
#include "../data/colors.hpp"
#include "../shared/key_event.hpp"

// Dialog colors
static constexpr u32 DIALOG_SHADOW_COLOR = Colors::BLACK;
static constexpr u32 DIALOG_BORDER_COLOR = Colors::WHITE;
static constexpr u32 DIALOG_BACKGROUND_COLOR = Colors::DARK_GRAY;
static constexpr u32 DIALOG_TITLE_COLOR = Colors::RED;
static constexpr u32 DIALOG_TEXT_COLOR = Colors::WHITE;
static constexpr u32 DIALOG_BUTTON_COLOR = Colors::RED;
static constexpr u32 DIALOG_BUTTON_TEXT_COLOR = Colors::WHITE;

class Dialog {
public:
  int x;
  int y;
  int width;
  int height;

  String title;
  String message;

  bool visible = false;

  Dialog(int x, int y, int width, int height, const String &title,
         const String &message)
      : x(x), y(y), width(width), height(height), title(title),
        message(message) {}

  void show() { visible = true; }

  void close() { visible = false; }

  void draw() {
    if (!visible)
      return;

    constexpr int TITLE_BAR_HEIGHT = 30;
    constexpr int PADDING = 16;
    constexpr int BUTTON_HEIGHT = 28;
    constexpr int BUTTON_WIDTH = 80;

    // Shadow
    Graphics::draw_rect(
        x + 5, y + 5, width, height,
        DIALOG_SHADOW_COLOR
    );

    // Outer dialog
    Graphics::draw_rect(
        x, y, width, height,
        DIALOG_BORDER_COLOR
    );

    // Main background
    Graphics::draw_rect(
        x + 2, y + 2,
        width - 4, height - 4,
        DIALOG_BACKGROUND_COLOR
    );

    // Title bar
    Graphics::draw_rect(
        x + 2, y + 2,
        width - 4, TITLE_BAR_HEIGHT,
        DIALOG_TITLE_COLOR
    );

    // Title
    Graphics::draw_string(
        title.c_str(),
        x + PADDING,
        y + 10,
        DIALOG_TEXT_COLOR
    );

    // Message
    Graphics::draw_string(
        message.c_str(),
        x + PADDING,
        y + TITLE_BAR_HEIGHT + 20,
        DIALOG_TEXT_COLOR
    );

    // Buttons area
    int button_y = y + height - BUTTON_HEIGHT - PADDING;

    // OK button
    int ok_x = x + width - BUTTON_WIDTH - PADDING;

    Graphics::draw_rect(
        ok_x, button_y,
        BUTTON_WIDTH, BUTTON_HEIGHT,
        DIALOG_BUTTON_COLOR
    );

    Graphics::draw_string(
        "OK",
        ok_x + 30,
        button_y + 9,
        DIALOG_BUTTON_TEXT_COLOR
    );
  }

  bool handle_key(KeyEvent &ev) {
    if (ev.keytype == KeyType::Escape ||
        ev.keytype == KeyType::Enter) {
      close();
      return true;
    }

    return false;
  }
};
