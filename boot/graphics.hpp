#pragma once

#include "../kernel/data/font.hpp"
#include "../kernel/shared/types.hpp"
#include "multiboot2.hpp"

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

inline void draw_char(char c, u32 x, u32 y, u32 color) {
  if (!Multiboot2::framebuffer.valid)
    return;

  if ((u8)c >= 128)
    return;

  const u8 *glyph = FONT[(u8)c];

  for (u32 row = 0; row < 8; row++) {
    u8 bits = glyph[row];

    for (u32 col = 0; col < 8; col++) {
      if (bits & (1 << (7 - col))) {
        put_pixel(x + col, y + row, color);
      }
    }
  }
}

inline void draw_string(const char *str, u32 x, u32 y, u32 color) {
  u32 i = 0;

  while (str[i] != '\0') {
    draw_char(str[i], x + i * CHARACTER_WIDTH, y, color);

    i++;
  }
}

} // namespace Graphics
