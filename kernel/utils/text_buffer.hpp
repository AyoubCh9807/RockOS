#pragma once

#include "../memory/heap.hpp"

class TextBuffer {
public:
  struct Cell {
    char c;
    u32 color;

    Cell(char c, u32 color) : c(c), color(color) {}
    Cell(char c) : c(c), color(0xFFFFFF) {}
  };

private:
  Cell *cells = nullptr;

  int columns = 0;
  int rows = 0;
  int size = 0;

  int cursor_position = 0;
  int input_start = 0;
  int input_end = 0;

public:
  TextBuffer() = default;

  TextBuffer(int columns, int rows) { resize(columns, rows); }

  void resize(int new_columns, int new_rows) {
    if (new_columns <= 0 || new_rows <= 0)
      return;

    if (new_columns == columns && new_rows == rows)
      return;

    if (cells)
      kfree(cells);

    columns = new_columns;
    rows = new_rows;
    size = columns * rows;

    cells = (Cell *)kmalloc(size * sizeof(Cell));

    clear();
  }

  int get_columns() const { return columns; }

  int get_rows() const { return rows; }

  int get_size() const { return size; }

  int get_cursor_position() const { return cursor_position; }

  int get_input_start() const { return input_start; }

  int get_input_end() const { return input_end; }

  void set_input_start(int value) { input_start = value; }

  void set_input_end(int value) { input_end = value; }

  Cell &get_cell(int index) { return cells[index]; }

  const Cell &get_cell(int index) const { return cells[index]; }

  void clear_cell(Cell &cell) {
    cell.c = ' ';
    cell.color = 0xFFFFFF;
  }

  void clear() {
    for (int i = 0; i < size; i++)
      clear_cell(cells[i]);

    cursor_position = 0;
    input_start = 0;
    input_end = 0;
  }

  void scroll() {
    for (int i = columns; i < size; i++)
      cells[i - columns] = cells[i];

    for (int i = size - columns; i < size; i++)
      clear_cell(cells[i]);

    cursor_position = size - columns;
  }

  void putchar(Cell cell) {
    if (size == 0)
      return;

    if (cell.c == '\b') {
      backspace();
      return;
    }

    if (cell.c == '\n') {
      newline();
      return;
    }

    if (cursor_position >= size)
      scroll();

    cells[cursor_position] = cell;
    cursor_position++;

    if (cursor_position >= size)
      scroll();
  }

  void newline() {
    int next = cursor_position + columns - (cursor_position % columns);

    if (next >= size)
      scroll();
    else
      cursor_position = next;
  }

  void backspace() {
    if (cursor_position <= input_start)
      return;

    cursor_position--;

    for (int i = cursor_position; i < input_end - 1; i++)
      cells[i] = cells[i + 1];

    clear_cell(cells[input_end - 1]);

    input_end--;
  }

  void insert_char(Cell cell) {
    if (cursor_position > input_end)
      return;

    if (input_end >= size)
      return;

    for (int i = input_end; i > cursor_position; i--)
      cells[i] = cells[i - 1];

    cells[cursor_position] = cell;

    cursor_position++;
    input_end++;
  }

  void move_left() {
    if (cursor_position > input_start)
      cursor_position--;
  }

  void move_right() {
    if (cursor_position < input_end)
      cursor_position++;
  }

  void begin_input() {
    input_start = cursor_position;
    input_end = cursor_position;
  }

  int get_input_text(char *output, int max_length) const {
    if (!output || max_length <= 0)
      return 0;

    int length = input_end - input_start;

    if (length >= max_length)
      length = max_length - 1;

    for (int i = 0; i < length; i++)
      output[i] = cells[input_start + i].c;

    output[length] = '\0';

    return length;
  }

  void clear_input() {
    while (cursor_position > input_start)
      backspace();
  }

  void set_input(const char *text) {
    clear_input();

    if (!text)
      return;

    for (int i = 0; text[i] != '\0'; i++)
      insert_char(Cell(text[i]));
  }
};
