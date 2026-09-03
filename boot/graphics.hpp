#pragma once

#include "multiboot2.hpp"

#include "../kernel/data/font.hpp"
#include "../kernel/shared/types.hpp"
#include "../kernel/utils/bit_utils.hpp"

namespace Graphics {

inline void put_pixel(u32 x, u32 y, u32 color) {
  if (!Multiboot2::framebuffer.valid)
    return;

  Framebuffer &fb = Multiboot2::framebuffer;

  if (x >= fb.width || y >= fb.height)
    return;

  u32 bytes_per_pixel = fb.bpp / 8;

  u8 *pixel = fb.address + y * fb.pitch + x * bytes_per_pixel;

  if (fb.bpp == 32) {
    *reinterpret_cast<u32 *>(pixel) = color;
  } else if (fb.bpp == 16) {
    u16 r = (color >> 16) & 0xFF;
    u16 g = (color >> 8) & 0xFF;
    u16 b = color & 0xFF;

    u16 rgb565 = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);

    *reinterpret_cast<u16 *>(pixel) = rgb565;
  }
}

inline void clear(u32 color) {
  if (!Multiboot2::framebuffer.valid)
    return;

  Framebuffer &fb = Multiboot2::framebuffer;

  for (u32 y = 0; y < fb.height; y++) {
    for (u32 x = 0; x < fb.width; x++) {
      put_pixel(x, y, color);
    }
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

} // namespace Graphics
