#pragma once

#include "../data/colors.hpp"
#include "string_utils.hpp"
#include "text_buffer.hpp"

#include "../gui/window.hpp"

class TerminalUtils {
private:
  TextBuffer buffer;

  u32 background_color = Colors::BLACK;
  u32 text_color = Colors::WHITE;

  bool cursor_visible = true;

  int content_width = 0;
  int content_height = 0;

  int calculate_columns(int width) const {
    return width / Graphics::CHARACTER_WIDTH;
  }

  int calculate_rows(int height) const {
    return height / Graphics::CHARACTER_HEIGHT;
  }

public:
  TerminalUtils() = default;

  void resize(int width, int height) {
    int columns = calculate_columns(width);
    int rows = calculate_rows(height);

    if (columns <= 0 || rows <= 0)
      return;

    if (columns == buffer.get_columns() && rows == buffer.get_rows())
      return;

    content_width = width;
    content_height = height;

    buffer.resize(columns, rows);
  }

  TextBuffer &get_buffer() { return buffer; }

  const TextBuffer &get_buffer() const { return buffer; }

  void clear() { buffer.clear(); }

  void putchar(char c, u32 color = Colors::WHITE) {
    buffer.putchar(TextBuffer::Cell(c, color));
  }

  void print(const char *text, u32 color = Colors::WHITE) {
    if (!text)
      return;

    for (int i = 0; text[i] != '\0'; i++)
      putchar(text[i], color);
  }

  void print_formatted(u32 color, const char *fmt, ...) {
    char buf[256];

    va_list args;
    va_start(args, fmt);

    StringUtils::vsnprintf(buf, sizeof(buf), fmt, args);

    va_end(args);

    print(buf, color);
  }

  void backspace() { buffer.backspace(); }

  void insert_char(char c, u32 color = Colors::WHITE) {
    buffer.insert_char(TextBuffer::Cell(c, color));
  }

  void move_left() { buffer.move_left(); }

  void move_right() { buffer.move_right(); }

  void begin_input() { buffer.begin_input(); }

  int get_cursor_position() const { return buffer.get_cursor_position(); }

  void render(Window &win) {
    win.clear(background_color);

    const int columns = buffer.get_columns();
    const int size = buffer.get_size();

    for (int i = 0; i < size; i++) {
      const TextBuffer::Cell &cell = buffer.get_cell(i);

      const int x = (i % columns) * Graphics::CHARACTER_WIDTH;

      const int y = (i / columns) * Graphics::CHARACTER_HEIGHT;

      if (cell.c != ' ') {
        char text[2] = {cell.c, '\0'};

        win.draw_string(text, x, y, cell.color);
      }
    }

    if (cursor_visible)
      draw_cursor(win);
  }

private:
  void draw_cursor(Window &win) {
    const int position = buffer.get_cursor_position();

    const int columns = buffer.get_columns();

    if (columns <= 0)
      return;

    const int x = (position % columns) * Graphics::CHARACTER_WIDTH;

    const int y = (position / columns) * Graphics::CHARACTER_HEIGHT;

    win.draw_rect(x, y, Graphics::CHARACTER_WIDTH, Graphics::CHARACTER_HEIGHT,
                  Colors::WHITE);

    char c = ' ';

    if (position < buffer.get_size())
      c = buffer.get_cell(position).c;

    char text[2] = {c, '\0'};

    win.draw_string(text, x, y, Colors::BLACK);
  }
};
