#pragma once

#include "../../data/system.hpp"
#include "../window.hpp"
#include "../window_app.hpp"

class CalculatorApp : public IWindowApp {
private:
  static constexpr int PADDING = 12;
  static constexpr int DISPLAY_H = 52;
  static constexpr int BUTTON_GAP = 6;
  static constexpr int BUTTON_ROWS = 5;
  static constexpr int BUTTON_COLS = 4;

  struct Button {
    const char *label;
  };

  Button buttons[BUTTON_ROWS][BUTTON_COLS] = {
      {{"7"}, {"8"}, {"9"}, {"/"}},   {{"4"}, {"5"}, {"6"}, {"*"}},
      {{"1"}, {"2"}, {"3"}, {"-"}},   {{"0"}, {"."}, {"C"}, {"+"}},
      {{"("}, {")"}, {"DEL"}, {"="}},
  };

  String expression = "";
  int res = 0;

  int button_width(const Window &win) const {
    int available = win.get_width() - PADDING * 2;
    return (available - BUTTON_GAP * (BUTTON_COLS - 1)) / BUTTON_COLS;
  }

  int button_height(const Window &win) const {
    int available =
        win.get_height() - PADDING * 2 - DISPLAY_H - BUTTON_GAP * BUTTON_ROWS;

    return available / BUTTON_ROWS;
  }

  int button_x(const Window &win, int col) const {
    return PADDING + col * (button_width(win) + BUTTON_GAP);
  }

  int button_y(const Window &win, int row) const {
    return PADDING + DISPLAY_H + BUTTON_GAP +
           row * (button_height(win) + BUTTON_GAP);
  }

  bool button_contains(const Window &win, int row, int col, int mouse_x,
                       int mouse_y) const {
    int x = button_x(win, col);
    int y = button_y(win, row);

    return mouse_x >= x && mouse_x < x + button_width(win) && mouse_y >= y &&
           mouse_y < y + button_height(win);
  }

  u32 button_color(const char *label) const {
    if (label[0] == '=')
      return Colors::RED;

    if (label[0] == '+' || label[0] == '-' || label[0] == '*' ||
        label[0] == '/')
      return Colors::GOLD;

    if (label[0] == 'C' || label[0] == 'D')
      return Colors::DARK_RED;

    return Colors::GRAY;
  }

  void draw_display(Window &win) {
    win.draw_rect(PADDING, PADDING, win.get_width() - PADDING * 2, DISPLAY_H,
                  Colors::WHITE);

    win.draw_string(expression.c_str(), PADDING + 10, PADDING + 17,
                    Colors::BLACK);
  }

  void draw_button(Window &win, int row, int col) {
    const Button &button = buttons[row][col];

    int x = button_x(win, col);
    int y = button_y(win, row);

    int width = button_width(win);
    int height = button_height(win);

    win.draw_rect(x, y, width, height, button_color(button.label));

    win.draw_string(button.label, x + width / 2 - 4, y + height / 2 - 6,
                    Colors::BLACK);
  }

  void draw_buttons(Window &win) {
    for (int row = 0; row < BUTTON_ROWS; row++) {
      for (int col = 0; col < BUTTON_COLS; col++)
        draw_button(win, row, col);
    }
  }

  void handle_button(const char *label) {
    if (label[0] == '=' && label[1] == '\0') {
      on_equals();
      return;
    }

    if (label[0] == 'C' && label[1] == '\0') {
      on_clear();
      return;
    }

    if (label[0] == 'D') {
      on_delete();
      return;
    }

    on_input(label[0]);
  }

protected:
  /*
   * Calculator backend hooks.
   *
   * The UI doesn't know how expressions are stored,
   * modified, or evaluated.
   */

  virtual void on_input(char value) { expression += value; }
  virtual void on_clear() { expression = ""; }

  virtual void on_delete() { expression.clear(); }
  virtual void on_equals() { expression.resolve_int(res); }

public:
  const char *name() const override { return "Calculator"; }

  void draw(Window &win) {
    win.clear(Colors::DARK_RED);

    draw_display(win);
    draw_buttons(win);
  }

  void on_create(Window &win) override { draw(win); }

  void on_draw(Window &win) override { draw(win); }

  void on_key(Window &win, const KeyEvent &ev) override {
    switch (ev.keytype) {
    case KeyType::Char:
      on_input(ev.scancode);
      break;

    case KeyType::BackSpace:
      on_delete();
      break;

    case KeyType::Enter:
      on_equals();
      break;

    default:
      return;
    }

    draw(win);
  }

  void on_mouse_event(Window &win, const MouseEvent &ev) override {
    if (ev.button_type != MouseButton::LEFT_BUTTON ||
        ev.event_type != MouseEventType::PRESS)
      return;

    int mouse_x = Mouse::get_x();
    int mouse_y = Mouse::get_y();

    if (!win.contains(mouse_x, mouse_y))
      return;

    for (int row = 0; row < BUTTON_ROWS; row++) {
      for (int col = 0; col < BUTTON_COLS; col++) {
        if (!button_contains(win, row, col, mouse_x, mouse_y))
          continue;

        handle_button(buttons[row][col].label);
        draw(win);
        return;
      }
    }
  }
};
