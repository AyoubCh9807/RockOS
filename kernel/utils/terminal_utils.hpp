#pragma once

#include "../../boot/graphics.hpp"
#include "../../boot/multiboot2.hpp"
#include "../core/rtc.hpp"
#include "../data/colors.hpp"
#include "../memory/heap.hpp"
#include "string_utils.hpp"

class TerminalUtils {
public:
  struct Cell {
    char c;
    u32 color;

    Cell(char c, u32 color) : c(c), color(color) {};
    Cell(char c) : c(c), color(0xFFFFFF) {};
  };

private:
  const int ROWS = Multiboot2::framebuffer.height / Graphics::CHARACTER_HEIGHT;
  const int COLUMNS = Multiboot2::framebuffer.width / Graphics::CHARACTER_WIDTH;
  const int SCREEN_SIZE = COLUMNS * ROWS;

  constexpr static int STATUS_BAR_ROWS = 5;
  const int TERMINAL_ROWS = ROWS - STATUS_BAR_ROWS;
  const int TERMINAL_SIZE = COLUMNS * TERMINAL_ROWS;

  int input_end = 0;

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
    for (int i = 0; i < TERMINAL_SIZE; i++) {
      clear_cell(cells[i]);
    }
    cursor_position = 0;
  }

  void scroll() {
    // Shift all rows up by one row
    for (int i = COLUMNS; i < TERMINAL_SIZE; i++) {
      cells[i - COLUMNS] = cells[i];
    }

    // Clear last row
    for (int i = TERMINAL_SIZE - COLUMNS; i < TERMINAL_SIZE; i++) {
      clear_cell(cells[i]);
    }

    // Put cursor on bottom left
    cursor_position = TERMINAL_SIZE - COLUMNS;
  }

  void backspace() {
    if (cursor_position == 0 || cursor_position > input_end)
      return;

    int old_cursor = cursor_position;

    cursor_position--;

    for (int i = cursor_position; i < input_end - 1; i++) {
      cells[i] = cells[i + 1];
    }

    input_end--;

    clear_cell(cells[input_end]);

    for (int i = cursor_position; i <= input_end; i++) {
      render_cell(i);
    }

    // Explicitly redraw the old cursor location (because if we dont, the
    // framebuffer still has the pixels of the cursor in the old cursor
    // position, we need to update whats being displayed by clearing the old
    // cell too)
    render_cell(old_cursor);

    render_cursor();
  }

  void putchar(Cell cell) {
    if (cursor_position < 0)
      return;
    if (cursor_position > TERMINAL_SIZE - 1)
      scroll();

    if (cell.c == '\n') {
      // Calculate the position where the cursor would be if the newline happens
      // (beginning of the next row)
      const int newline_pos =
          cursor_position + COLUMNS - (cursor_position % COLUMNS);
      if (newline_pos > TERMINAL_SIZE - 1)
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

  void insert_char(Cell cell, int end) {
    if (cursor_position > end)
      return;

    for (int i = end; i > cursor_position; i--) {
      cells[i] = cells[i - 1];
    }

    cells[cursor_position] = cell;

    // Redraw everything from the insertion point to the end.
    for (int i = cursor_position; i <= end; i++) {
      render_cell(i);
    }

    cursor_position++;

    render_cursor();
  }

  void move_left() {
    if (cursor_position % COLUMNS > 0) {
      cursor_position--;
      render_cell(cursor_position + 1);
      render_cursor();
    };
  }

  void move_right() {
    if (cursor_position < TERMINAL_SIZE - 1) {
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
    int y = (index / COLUMNS + STATUS_BAR_ROWS) * Graphics::CHARACTER_HEIGHT;

    Graphics::draw_rect(x, y, Graphics::CHARACTER_WIDTH,
                        Graphics::CHARACTER_HEIGHT, 0x000000);
  }

  void render_cell(int index) {
    int x = (index % COLUMNS) * Graphics::CHARACTER_WIDTH;
    int y = (index / COLUMNS + STATUS_BAR_ROWS) * Graphics::CHARACTER_HEIGHT;

    // Erase the physical cell first
    Graphics::draw_rect(x, y, Graphics::CHARACTER_WIDTH,
                        Graphics::CHARACTER_HEIGHT, 0x000000);

    // Draw its actual contents
    Graphics::draw_char(cells[index].c, x, y, cells[index].color);
  }

  void render() {

    Graphics::clear(0x000000);

    if (cursor_position > TERMINAL_SIZE - 1)
      scroll();

    for (int i = 0; i < TERMINAL_SIZE; i++) {
      Graphics::draw_char(cells[i].c, (i % COLUMNS) * Graphics::CHARACTER_WIDTH,
                          (i / COLUMNS + STATUS_BAR_ROWS) *
                              Graphics::CHARACTER_HEIGHT,
                          cells[i].color);
    }

    render_cursor();
  }

  void render_cursor() {

    const int cursor_x =
        (cursor_position % COLUMNS) * Graphics::CHARACTER_WIDTH;
    const int cursor_y = (cursor_position / COLUMNS + STATUS_BAR_ROWS) *
                         Graphics::CHARACTER_HEIGHT;
    Graphics::draw_bitmap(CURSOR_CHAR, cursor_x, cursor_y, 0xFFFFFF);
  }

  void draw_status_char(char c, int x, int y, u32 color) {
    Graphics::draw_rect(x, y, Graphics::CHARACTER_WIDTH,
                        Graphics::CHARACTER_HEIGHT, 0x000000);

    Graphics::draw_char(c, x, y, 0xFFFFFF);
  }

  void draw_bar() {
    size_t used = heap.get_used();

    size_t whole = used / (1024 * 1024);
    size_t fraction = ((used % (1024 * 1024)) * 10) / (1024 * 1024);

    String heap_text =
        StringUtils::format("HEAP: %d.%d / 4 MiB", whole, fraction);

    int cpu = (int)Timer::get_cpu_usage();
    String uptime = Timer::get_formatted_time();
    String full_date = RTC::get_full_time();

    String system_text = String::format("CPU: %d | %s | date: %s", cpu,
                                        uptime.c_str(), full_date.c_str());

    // Row 0
    int x = 0;
    for (int i = 0; system_text[i] != '\0'; i++) {
      draw_status_char(system_text[i], x, 0, 0xFFFFFF);

      x += Graphics::CHARACTER_WIDTH;
    }

    // Row 2
    x = 0;
    for (int i = 0; heap_text[i] != '\0'; i++) {
      draw_status_char(heap_text[i], x, Graphics::CHARACTER_HEIGHT * 2,
                       0xFFFFFF);

      x += Graphics::CHARACTER_WIDTH;
    }

    // Row 1
    size_t percentage = (used * 100) / (4 * 1024 * 1024);

    int bar_width = 400;
    int filled_width = (bar_width * percentage) / 100;

    Graphics::draw_rect(10, Graphics::CHARACTER_HEIGHT, filled_width,
                        Graphics::CHARACTER_HEIGHT, Colors::DARK_RED);

    Graphics::draw_rect(10 + filled_width, Graphics::CHARACTER_HEIGHT,
                        bar_width - filled_width, Graphics::CHARACTER_HEIGHT,
                        Colors::GRAY);

    // Row 3
    x = 0;
    for (int i = 0; i < COLUMNS; i++) {
      draw_status_char('_', i * Graphics::CHARACTER_WIDTH,
                       Graphics::CHARACTER_HEIGHT * (STATUS_BAR_ROWS - 2),
                       Colors::GOLD);
    }
  }

  static void update_status_bar() {
    if (global_terminal)
      global_terminal->draw_bar();
  }

  const int get_cursor_position() const { return cursor_position; }
};
