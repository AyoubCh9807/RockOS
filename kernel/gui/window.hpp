#pragma once

#include "../data/colors.hpp"
#include "../data/font.hpp"
#include "../shared/types.hpp"

/* A Window owns a private pixel buffer. Apps draw into that buffer,
   never directly onto the real framebuffer, the WindowManager is the
   only thing that ever touches the actual screen, by compositing
   every window's buffer onto it in z-order. This separation is what
   will make Phase 2 easy later, once a window's app lives in its own
   process, the process just needs to hand pixels to the kernel
   somehow (a syscall), it never needs direct framebuffer access. */
class Window {
private:
  u32 *pixels;

  static constexpr int CHAR_WIDTH = 8;
  static constexpr int CHAR_HEIGHT = 8;

public:
  int x, y;
  int width, height;
  const char *title;

  /* Higher draws on top. WindowManager keeps windows sorted by this
     when compositing and when picking which window gets focus. */
  int z_order;

  bool focused;

  Window(int x, int y, int width, int height, const char *title)
      : x(x), y(y), width(width), height(height), title(title), z_order(0),
        focused(false) {

    pixels = new u32[width * height];

    if (!pixels)
      return;

    clear(0x000000);
  }

  ~Window() { delete[] pixels; }

  void clear(u32 color) {
    for (int i = 0; i < width * height; i++)
      pixels[i] = color;
  }

  void set_pixel(int local_x, int local_y, u32 color) {
    if (local_x < 0 || local_x >= width || local_y < 0 || local_y >= height)
      return;
    pixels[local_y * width + local_x] = color;
  }

  u32 get_pixel(int local_x, int local_y) const {
    if (local_x < 0 || local_x >= width || local_y < 0 || local_y >= height)
      return 0;
    return pixels[local_y * width + local_x];
  }

  /* Same font and bit layout Graphics::draw_char uses, but plotted
     through set_pixel above instead of Graphics::put_pixel, so the
     glyph lands in this window's own buffer at window-local
     coordinates instead of on the real screen at absolute ones. */
  void draw_char(char c, int local_x, int local_y, u32 color) {
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
    int i = 0;
    while (str[i] != '\0') {
      draw_char(str[i], local_x + i * CHAR_WIDTH, local_y, color);
      i++;
    }
  }

  void draw_rect(int x, int y, int width, int height, u32 color) {
    for (int py = y; py < y + height; py++) {
      for (int px = x; px < x + width; px++) {
        set_pixel(px, py, color);
      }
    }
  }

  void draw_border(int thickness, u32 color) {
    // Top and bottom
    draw_rect(0, 0, width, thickness, color);
    draw_rect(0, height - thickness, width, thickness, color);

    // Left and right
    draw_rect(0, 0, thickness, height, color);
    draw_rect(width - thickness, 0, thickness, height, color);
  }

  const u32 *get_buffer() const { return pixels; }
};
