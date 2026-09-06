#pragma once

#include "multiboot2.hpp"

#include "../kernel/data/font.hpp"
#include "../kernel/shared/types.hpp"
#include "graphic_colors.hpp"

namespace Graphics {

constexpr u32 MAX_WIDTH = 1920;
constexpr u32 MAX_HEIGHT = 1080;

inline u32 back_buffer[MAX_WIDTH * MAX_HEIGHT];
inline u64 back_buffer_size = 0;

inline bool init_back_buffer() {
  if (!Multiboot2::framebuffer.valid)
    return false;

  Framebuffer &fb = Multiboot2::framebuffer;

  if (fb.bpp != 32)
    return false;

  if (fb.width > MAX_WIDTH || fb.height > MAX_HEIGHT)
    return false;

  back_buffer_size = static_cast<u64>(fb.width) * fb.height;

  return true;
}

inline void put_pixel(u32 x, u32 y, u32 color) {
  if (!Multiboot2::framebuffer.valid)
    return;

  Framebuffer &fb = Multiboot2::framebuffer;

  if (!back_buffer)
    return;

  if (x >= fb.width || y >= fb.height)
    return;

  back_buffer[y * fb.width + x] = color;
}

inline void clear(u32 color) {
  if (!Multiboot2::framebuffer.valid || !back_buffer)
    return;

  Framebuffer &fb = Multiboot2::framebuffer;
  u32 total_size = fb.width * fb.height;

  for (u64 i = 0; i < total_size; i++) {
    back_buffer[i] = color;
  }
}

inline void draw_rect(u32 x, u32 y, u32 w, u32 h, u32 color) {
  for (int i = x; i < x + w; i++) {
    for (int j = y; j < y + h; j++) {
      put_pixel(i, j, color);
    }
  }
}

inline void draw_line(u32 x, u32 y, u32 w, u32 color) {
  for (int i = x; i < x + w; i++) {
    put_pixel(i, y, color);
  }
}

constexpr int CHARACTER_WIDTH = 8;
constexpr int CHARACTER_HEIGHT = 8;

static constexpr int FONT_BITMAP_ROWS = 8;
static constexpr int FONT_BITMAP_COLS = 8;

inline void draw_bitmap(const u8 *bitmap, int x, int y, u32 color) {
  for (int i = 0; i < FONT_BITMAP_ROWS; i++) {

    const u8 bm = bitmap[i];

    for (int j = 0; j < FONT_BITMAP_COLS; j++) {
      if (bm & (1 << (FONT_BITMAP_COLS - 1 - j)))
        put_pixel(x + j, y + i, color);
    }
  }
}

inline void draw_char(char c, u32 x, u32 y, u32 color) {
  if (!Multiboot2::framebuffer.valid)
    return;

  if ((u8)c >= 128)
    return;

  const u8 *glyph = FONT[(u8)c];

  draw_bitmap(glyph, x, y, color);
}

inline void draw_string(const char *str, u32 x, u32 y, u32 color) {
  u32 i = 0;

  while (str[i] != '\0') {
    draw_char(str[i], x + i * CHARACTER_WIDTH, y, color);

    i++;
  }
}

inline void draw_bitmap(const u8 *bitmap, u32 x, u32 y, u32 width, u32 height,
                        u32 color) {
  for (u32 i = 0; i < height; i++) {
    u8 bm = bitmap[i];

    for (u32 j = 0; j < width; j++) {
      if (bm & (1 << (width - 1 - j)))
        put_pixel(x + j, y + i, color);
    }
  }
}

inline void draw_image(const u32 *pixels, u32 x, u32 y, u32 width, u32 height) {
  if (!pixels)
    return;

  for (u32 py = 0; py < height; py++) {
    for (u32 px = 0; px < width; px++) {
      u32 color = pixels[py * width + px];

      // 0 alpha = transparent
      if ((color >> 24) == 0)
        continue;

      put_pixel(x + px, y + py, color);
    }
  }
}

inline void present() {
  if (!Multiboot2::framebuffer.valid)
    return;

  if (!back_buffer)
    return;

  Framebuffer &fb = Multiboot2::framebuffer;

  if (fb.bpp != 32)
    return;

  for (u32 y = 0; y < fb.height; y++) {
    u32 *dst = reinterpret_cast<u32 *>(fb.address + y * fb.pitch);

    u32 *src = back_buffer + y * fb.width;

    for (u32 x = 0; x < fb.width; x++) {
      dst[x] = src[x];
    }
  }
}

static void draw_cursor(int x, int y) {
  // The cursor is drawn relative to its tip at (x, y).
  static constexpr int cursor[] = {
      0, 0,

      0, 1,  1, 1,  2, 1,  3, 1,  0, 2,  1, 2,  3, 2,  4, 2,  0, 3,
      1, 3,  4, 3,  5, 3,  0, 4,  1, 4,  2, 4,  3, 4,  0, 5,  1, 5,
      2, 5,  3, 5,  4, 5,  0, 6,  1, 6,  2, 6,  0, 7,  1, 7,  2, 7,
      3, 7,  4, 7,  0, 8,  1, 8,  2, 8,  4, 8,  5, 8,  0, 9,  1, 9,
      2, 9,  2, 10, 3, 10, 4, 10, 3, 11, 4, 11, 5, 11, 4, 12, 5, 12,
      6, 12, 5, 13, 6, 13, 7, 13, 6, 14, 7, 14, 7, 15, 8, 15,
  };
  constexpr int point_count = sizeof(cursor) / sizeof(cursor[0]) / 2;

  for (int i = 0; i < point_count; ++i) {
    int px = x + cursor[i * 2];
    int py = y + cursor[i * 2 + 1];

    put_pixel(px, py, WHITE);
  }
}

} // namespace Graphics
