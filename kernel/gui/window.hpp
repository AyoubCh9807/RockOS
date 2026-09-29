#pragma once

#include "../data/colors.hpp"
#include "../data/font.hpp"
#include "../shared/types.hpp"

class Window {

  static constexpr int WINDOW_TITLE_BAR_HEIGHT = 24;

private:
  u32 *pixels = nullptr;

  int border_thickness = 2;
  u32 border_color = Colors::GOLD;

  int restore_x = 0;
  int restore_y = 0;
  int restore_width = 0;
  int restore_height = 0;

  bool maximized = false;

  static constexpr int CHAR_WIDTH = 8;
  static constexpr int CHAR_HEIGHT = 8;

public:
  int x;
  int y;
  int width;
  int height;

  const char *title;

  enum class Button { NONE, MINIMIZE, MAXIMIZE, CLOSE };

  int z_order;
  bool focused;
  bool minimized = false;

  Window(int x, int y, int width, int height, const char *title)
      : x(x), y(y), width(width), height(height), title(title), z_order(0),
        focused(false) {

    if (width <= 0 || height <= 0) {
      this->width = 0;
      this->height = 0;
      return;
    }

    pixels = new u32[width * height];

    if (!pixels)
      return;

    clear(0x000000);
  }

  ~Window() {
    delete[] pixels;
    pixels = nullptr;
  }

  void minimize() { minimized = true; }

  void maximize(int screen_width, int screen_height, int taskbar_height) {
    if (maximized || minimized)
      return;

    const int new_width = screen_width;
    const int new_height = screen_height - taskbar_height;

    if (new_width <= 0 || new_height <= 0)
      return;

    u32 *new_pixels = new u32[new_width * new_height];

    if (!new_pixels)
      return;

    restore_x = x;
    restore_y = y;
    restore_width = width;
    restore_height = height;

    delete[] pixels;

    pixels = new_pixels;

    width = new_width;
    height = new_height;

    x = 0;
    y = 0;

    clear(0x000000);

    maximized = true;
  }

  void restore() {
    if (!maximized)
      return;

    x = restore_x;
    y = restore_y;

    if (!resize(restore_width, restore_height))
      return;

    maximized = false;
  }

  bool is_maximized() const { return maximized; }

  bool resize(int new_width, int new_height) {
    if (new_width <= 0 || new_height <= 0)
      return false;

    u32 *new_pixels = new u32[new_width * new_height];

    if (!new_pixels)
      return false;

    delete[] pixels;

    pixels = new_pixels;

    width = new_width;
    height = new_height;

    clear(0x000000);

    return true;
  }

  void clear(u32 color) {
    if (!pixels)
      return;

    for (int i = 0; i < width * height; i++)
      pixels[i] = color;
  }

  void set_pixel(int local_x, int local_y, u32 color) {
    if (!pixels)
      return;

    if (local_x < 0 || local_x >= width || local_y < 0 || local_y >= height)
      return;

    pixels[local_y * width + local_x] = color;
  }

  u32 get_pixel(int local_x, int local_y) const {
    if (!pixels)
      return 0;

    if (local_x < 0 || local_x >= width || local_y < 0 || local_y >= height)
      return 0;

    return pixels[local_y * width + local_x];
  }

  void draw_char(char c, int local_x, int local_y, u32 color) {
    if (!pixels)
      return;

    if ((u8)c >= 128)
      return;

    const u8 *glyph = Graphics::FONT[(u8)c];

    for (int row = 0; row < CHAR_HEIGHT; row++) {
      const u8 bits = glyph[row];

      for (int col = 0; col < CHAR_WIDTH; col++) {
        if (bits & (1 << (CHAR_WIDTH - 1 - col)))
          set_pixel(local_x + col, local_y + row, color);
      }
    }
  }

  void draw_string(const char *str, int local_x, int local_y, u32 color) {
    if (!str)
      return;

    int i = 0;

    while (str[i] != '\0') {
      draw_char(str[i], local_x + i * CHAR_WIDTH, local_y, color);
      i++;
    }
  }

  void draw_rect(int x, int y, int width, int height, u32 color) {
    if (!pixels)
      return;

    if (width <= 0 || height <= 0)
      return;

    for (int py = y; py < y + height; py++) {
      for (int px = x; px < x + width; px++)
        set_pixel(px, py, color);
    }
  }

  constexpr bool contains(int mouse_x, int mouse_y) const {
    if (!pixels)
      return false;

    return mouse_x >= x && mouse_x < x + width && mouse_y >= y &&
           mouse_y < y + height;
  }

  const u32 *get_buffer() const { return pixels; }

  int get_width() const { return width; }

  int get_height() const { return height; }

  int get_client_width() const { return width - 2 * border_thickness; }

  int get_client_height() const {
    return height - WINDOW_TITLE_BAR_HEIGHT - 2 * border_thickness;
  }

  constexpr int get_border_thickness() const { return border_thickness; }

  constexpr u32 get_border_color() const { return border_color; }
};
