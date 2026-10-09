#pragma once

#include "../../data/system.hpp"
#include "../window.hpp"
#include "../window_app.hpp"

class CalculatorApp : public IWindowApp {
private:
  static constexpr int PADDING = 14;
  static constexpr int DISPLAY_H = 76;
  static constexpr int BUTTON_GAP = 8;
  static constexpr int BUTTON_ROWS = 5;
  static constexpr int BUTTON_COLS = 4;
  static constexpr int MAX_EXPR = 32;
  static constexpr int GLYPH = 8;

  static constexpr u32 COL_BG = 0x1E1E2E;
  static constexpr u32 COL_DISPLAY = 0x11111B;
  static constexpr u32 COL_ACCENT = 0xF5A524;
  static constexpr u32 COL_TEXT = 0xF2F2F7;
  static constexpr u32 COL_DIM = 0x8A8AA0;
  static constexpr u32 COL_DARK_TEXT = 0x1E1E2E;

  enum class Kind { NUM, OP, CLEAR, DEL, EQUALS };

  struct Key {
    const char *label;
    Kind kind;
  };

  Key keys[BUTTON_ROWS][BUTTON_COLS] = {
      {{"7", Kind::NUM}, {"8", Kind::NUM}, {"9", Kind::NUM}, {"/", Kind::OP}},
      {{"4", Kind::NUM}, {"5", Kind::NUM}, {"6", Kind::NUM}, {"*", Kind::OP}},
      {{"1", Kind::NUM}, {"2", Kind::NUM}, {"3", Kind::NUM}, {"-", Kind::OP}},
      {{"0", Kind::NUM}, {".", Kind::NUM}, {"C", Kind::CLEAR}, {"+", Kind::OP}},
      {{"(", Kind::OP},
       {")", Kind::OP},
       {"DEL", Kind::DEL},
       {"=", Kind::EQUALS}},
  };

  String expression = "";
  int res = 0;
  char result_text[16] = {0};
  bool show_result = false;

  static int str_len(const char *s) {
    int n = 0;
    while (s && s[n])
      n++;
    return n;
  }

  // The window buffer includes the title bar and border, so skip past them.
  int origin_x(const Window &win) const { return win.get_border_thickness(); }

  int origin_y(const Window &win) const {
    return win.get_height() - win.get_client_height() -
           win.get_border_thickness();
  }

  int content_w(const Window &win) const { return win.get_client_width(); }

  int content_h(const Window &win) const { return win.get_client_height(); }

  int button_width(const Window &win) const {
    int available = content_w(win) - PADDING * 2;
    int w = (available - BUTTON_GAP * (BUTTON_COLS - 1)) / BUTTON_COLS;
    return w > 1 ? w : 1;
  }

  int button_height(const Window &win) const {
    int available =
        content_h(win) - PADDING * 2 - DISPLAY_H - BUTTON_GAP * BUTTON_ROWS;
    int h = available / BUTTON_ROWS;
    return h > 1 ? h : 1;
  }

  int button_x(const Window &win, int col) const {
    return origin_x(win) + PADDING + col * (button_width(win) + BUTTON_GAP);
  }

  int button_y(const Window &win, int row) const {
    return origin_y(win) + PADDING + DISPLAY_H + BUTTON_GAP +
           row * (button_height(win) + BUTTON_GAP);
  }

  bool button_contains(const Window &win, int row, int col, int px,
                       int py) const {
    int x = button_x(win, col);
    int y = button_y(win, row);

    return px >= x && px < x + button_width(win) && py >= y &&
           py < y + button_height(win);
  }

  u32 key_color(Kind kind) const {
    switch (kind) {
    case Kind::OP:
      return COL_ACCENT;
    case Kind::CLEAR:
      return 0xE5484D;
    case Kind::DEL:
      return 0x6E56CF;
    case Kind::EQUALS:
      return 0x30A46C;
    default:
      return 0x313244;
    }
  }

  u32 key_shadow(Kind kind) const {
    switch (kind) {
    case Kind::OP:
      return 0xB97A12;
    case Kind::CLEAR:
      return 0xA3282C;
    case Kind::DEL:
      return 0x4A3A94;
    case Kind::EQUALS:
      return 0x1F7049;
    default:
      return 0x23232F;
    }
  }

  u32 key_text(Kind kind) const {
    return kind == Kind::OP ? COL_DARK_TEXT : COL_TEXT;
  }

  void draw_text(Window &win, const char *str, int x, int y, int scale,
                 u32 color) {
    if (scale <= 1) {
      win.draw_string(str, x, y, color);
      return;
    }

    for (int i = 0; str[i]; i++) {
      u8 c = (u8)str[i];
      if (c >= 128)
        continue;

      const u8 *glyph = Graphics::FONT[c];

      for (int row = 0; row < GLYPH; row++) {
        for (int col = 0; col < GLYPH; col++) {
          if (glyph[row] & (0x80 >> col))
            win.draw_rect(x + (i * GLYPH + col) * scale, y + row * scale, scale,
                          scale, color);
        }
      }
    }
  }

  void draw_text_right(Window &win, const char *str, int right, int y,
                       int scale, int max_w, u32 color) {
    int n = str_len(str);
    int max_chars = max_w / (GLYPH * scale);
    int start = n > max_chars ? n - max_chars : 0;
    int shown = n - start;

    draw_text(win, str + start, right - shown * GLYPH * scale, y, scale, color);
  }

  void draw_display(Window &win) {
    int bx = origin_x(win) + PADDING;
    int by = origin_y(win) + PADDING;
    int bw = content_w(win) - PADDING * 2;

    win.draw_rect(bx, by, bw, DISPLAY_H, COL_DISPLAY);
    win.draw_rect(bx, by + DISPLAY_H - 2, bw, 2, COL_ACCENT);

    int right = bx + bw - 10;
    int inner_w = bw - 20;
    int big_y = by + DISPLAY_H - 34;
    const char *expr = expression.c_str();

    if (show_result) {
      draw_text_right(win, expr, right, by + 10, 1, inner_w, COL_DIM);
      draw_text_right(win, result_text, right, big_y, 2, inner_w, COL_TEXT);
    } else if (str_len(expr) == 0) {
      draw_text_right(win, "0", right, big_y, 2, inner_w, COL_DIM);
    } else {
      draw_text_right(win, expr, right, big_y, 2, inner_w, COL_TEXT);
    }
  }

  void draw_button(Window &win, int row, int col) {
    const Key &key = keys[row][col];

    int x = button_x(win, col);
    int y = button_y(win, row);
    int w = button_width(win);
    int h = button_height(win);

    win.draw_rect(x, y, w, h, key_color(key.kind));

    if (h > 10)
      win.draw_rect(x, y + h - 3, w, 3, key_shadow(key.kind));

    int text_w = str_len(key.label) * GLYPH;
    draw_text(win, key.label, x + (w - text_w) / 2, y + (h - GLYPH) / 2 - 1, 1,
              key_text(key.kind));
  }

  void draw_buttons(Window &win) {
    for (int row = 0; row < BUTTON_ROWS; row++) {
      for (int col = 0; col < BUTTON_COLS; col++)
        draw_button(win, row, col);
    }
  }

  void set_result(int v) {
    char tmp[16];
    int n = 0;
    unsigned int u = v < 0 ? 0u - (unsigned int)v : (unsigned int)v;

    do {
      tmp[n++] = (char)('0' + u % 10);
      u /= 10;
    } while (u && n < 12);

    int i = 0;
    if (v < 0)
      result_text[i++] = '-';
    while (n)
      result_text[i++] = tmp[--n];
    result_text[i] = '\0';
  }

  void press_input(char c) {
    if (str_len(expression.c_str()) >= MAX_EXPR)
      return;

    show_result = false;
    on_input(c);
  }

  void press_clear() {
    show_result = false;
    on_clear();
  }

  void press_delete() {
    show_result = false;
    on_delete();
  }

  void press_equals() {
    if (str_len(expression.c_str()) == 0)
      return;

    on_equals();
    set_result(res);
    show_result = true;
  }

  void handle_key(const Key &key) {
    switch (key.kind) {
    case Kind::CLEAR:
      press_clear();
      break;
    case Kind::DEL:
      press_delete();
      break;
    case Kind::EQUALS:
      press_equals();
      break;
    default:
      press_input(key.label[0]);
      break;
    }
  }

  bool is_calc_char(char c) const {
    return (c >= '0' && c <= '9') || c == '+' || c == '-' || c == '*' ||
           c == '/' || c == '.' || c == '(' || c == ')';
  }

  void redraw(Window &win) {
    win.clear(COL_BG);

    draw_display(win);
    draw_buttons(win);
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

  virtual void on_delete() {
    char tmp[MAX_EXPR + 1];
    const char *s = expression.c_str();
    int n = 0;

    while (s[n] && n < MAX_EXPR) {
      tmp[n] = s[n];
      n++;
    }

    expression.clear();

    for (int i = 0; i < n - 1; i++)
      expression += tmp[i];
  }

  virtual void on_equals() { expression.resolve_int(res); }

public:
  const char *name() const override { return "Calculator"; }

  void on_create(Window &win) override { redraw(win); }

  void on_draw(Window &win) override { redraw(win); }

  void on_key(Window &win, const KeyEvent &ev) override {
    switch (ev.keytype) {
    case KeyType::Char:
      // Swap scancode for your translated char field if you have one.
      if (!is_calc_char((char)ev.scancode))
        return;
      press_input((char)ev.scancode);
      break;

    case KeyType::BackSpace:
      press_delete();
      break;

    case KeyType::Enter:
      press_equals();
      break;

    default:
      return;
    }

    redraw(win);
  }

  void on_mouse_event(Window &win, const MouseEvent &ev) override {
    if (ev.button_type != MouseButton::LEFT_BUTTON ||
        ev.event_type != MouseEventType::PRESS)
      return;

    int mouse_x = Mouse::get_x();
    int mouse_y = Mouse::get_y();

    if (!win.contains(mouse_x, mouse_y))
      return;

    int px = mouse_x - win.x;
    int py = mouse_y - win.y;

    for (int row = 0; row < BUTTON_ROWS; row++) {
      for (int col = 0; col < BUTTON_COLS; col++) {
        if (!button_contains(win, row, col, px, py))
          continue;

        handle_key(keys[row][col]);
        redraw(win);
        return;
      }
    }
  }
};
