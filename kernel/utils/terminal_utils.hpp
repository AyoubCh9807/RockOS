#pragma once

#include "../../boot/graphics.hpp"
#include "../../boot/multiboot2.hpp"
#include "string_utils.hpp"

class TerminalUtils {

private:
  const int ROWS = Multiboot2::framebuffer.height / Graphics::CHARACTER_HEIGHT;
  const int COLUMNS = Multiboot2::framebuffer.width / Graphics::CHARACTER_WIDTH;
  const int SCREEN_SIZE = COLUMNS * ROWS;

  int input_end = 0;

  struct Cell {
    char c;
    u32 color;

    Cell(char c, u32 color) : c(c), color(color) {};
    Cell(char c) : c(c), color(0xFFFFFF) {};
  };

  int cursor_position = 0;
  Cell *cells;

  static constexpr u8 CURSOR_CHAR[8] = {
      0b00011000, 0b00011000, 0b00011000, 0b00011000,
      0b00011000, 0b00011000, 0b00011000, 0b00011000,
  };

  inline static TerminalUtils *global_terminal = nullptr;

public:
  TerminalUtils() {
    cells = (Cell *)kmalloc(SCREEN_SIZE * sizeof(Cell));
    for (int i = 0; i < SCREEN_SIZE; i++) {
      cells[i] = Cell(' ', 0xFFFFFF);
    }
    global_terminal = this;
  }

  void set_input_end(int val) { input_end = val; }

  void clear_cell(Cell &cell) {
    cell.c = ' ';
    cell.color = 0xFFFFFF;
  }

  void clear() {
    for (int i = 0; i < SCREEN_SIZE; i++) {
      clear_cell(cells[i]);
    }
    cursor_position = 0;
  }

  void scroll() {
    // Shift all rows up by one row
    for (int i = COLUMNS; i < SCREEN_SIZE; i++) {
      cells[i - COLUMNS] = cells[i];
    }

    // Clear last row
    for (int i = SCREEN_SIZE - COLUMNS; i < SCREEN_SIZE; i++) {
      clear_cell(cells[i]);
    }

    // Put cursor on bottom left
    cursor_position = SCREEN_SIZE - COLUMNS;
  }

  void backspace() {
    if (cursor_position == 0 || cursor_position > input_end)
      return;

    cursor_position--;

    for (int i = cursor_position; i < input_end; i++) {
      cells[i] = cells[i + 1];
    }

    clear_cell(cells[input_end]);

    for (int i = cursor_position; i <= input_end; i++) {
      render_cell(i);
    }

    render_cursor();
  }

  void putchar(Cell cell) {
    if (cursor_position < 0)
      return;
    if (cursor_position > SCREEN_SIZE - 1)
      scroll();

    if (cell.c == '\n') {
      // Calculate the position where the cursor would be if the newline happens
      // (beginning of the next row)
      const int newline_pos =
          cursor_position + COLUMNS - (cursor_position % COLUMNS);
      if (newline_pos > SCREEN_SIZE - 1)
        scroll();
      else
        cursor_position = newline_pos;
      render();
    } else if (cell.c == '\b') {
      backspace();
    } else {

      cells[cursor_position] = cell;
      render_cell(cursor_position);
      cursor_position++;
      render_cursor();
    }
  }

  void move_left() {
    if (cursor_position % COLUMNS > 0) {
      cursor_position--;
      render_cell(cursor_position + 1);
      render_cursor();
    };
  }

  void move_right() {
    if (cursor_position < SCREEN_SIZE - 1) {
      cursor_position++;
      render_cell(cursor_position - 1);
      render_cursor();
    }
  }

  // the shell implements the up and down arrows for history navigation so i
  // dont see a point in implementing these void move_up() {} void move_down()
  // {}

  void print(const char *str, const u32 color) {
    if (!str)
      return;

    for (int i = 0; str[i] != '\0'; i++) {
      putchar(Cell(str[i], color));
    }
  }

  static void static_print(const char *str, const u32 color) {
    global_terminal->print(str, color);
  }

  void print_number(int n) {
    char buf[16];
    StringUtils::print_number_into(buf, sizeof(buf), n);
    print(buf, 0xFFFFFF);
  }

  void clear_cell_screen(int index) {
    int x = (index % COLUMNS) * Graphics::CHARACTER_WIDTH;
    int y = (index / COLUMNS) * Graphics::CHARACTER_HEIGHT;

    Graphics::draw_rect(x, y, Graphics::CHARACTER_WIDTH,
                        Graphics::CHARACTER_HEIGHT, 0x000000);
  }

  void render_cell(int index) {
    int x = (index % COLUMNS) * Graphics::CHARACTER_WIDTH;
    int y = (index / COLUMNS) * Graphics::CHARACTER_HEIGHT;

    // Erase the physical cell first
    Graphics::draw_rect(x, y, Graphics::CHARACTER_WIDTH,
                        Graphics::CHARACTER_HEIGHT, 0x000000);

    // Draw its actual contents
    Graphics::draw_char(cells[index].c, x, y, cells[index].color);
  }

  void render() {

    Graphics::clear(0x000000);

    if (cursor_position > SCREEN_SIZE - 1)
      scroll();

    for (int i = 0; i < SCREEN_SIZE; i++) {
      Graphics::draw_char(cells[i].c, (i % COLUMNS) * Graphics::CHARACTER_WIDTH,
                          (i / COLUMNS) * Graphics::CHARACTER_HEIGHT,
                          cells[i].color);
    }

    render_cursor();
  }

  void render_cursor() {

    const int cursor_x =
        (cursor_position % COLUMNS) * Graphics::CHARACTER_WIDTH;
    const int cursor_y =
        (cursor_position / COLUMNS) * Graphics::CHARACTER_HEIGHT;
    Graphics::draw_bitmap(CURSOR_CHAR, cursor_x, cursor_y, 0xFFFFFF);
  }

  const int get_cursor_position() const { return cursor_position; }
};
