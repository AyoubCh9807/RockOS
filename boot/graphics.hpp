#pragma once

#include "multiboot2.hpp"

#include "../kernel/data/font.hpp"
#include "../kernel/desktop/cursor.hpp"
#include "../kernel/shared/types.hpp"
#include "../kernel/utils/math_utils.hpp"
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

  if (x >= fb.width || y >= fb.height)
    return;

  back_buffer[y * fb.width + x] = color;
}

inline void clear(u32 color) {
  if (!Multiboot2::framebuffer.valid)
    return;

  Framebuffer &fb = Multiboot2::framebuffer;
  u32 total_size = fb.width * fb.height;

  for (u32 i = 0; i < total_size; i++) {
    back_buffer[i] = color;
  }
}

void draw_rect(int x, int y, int width, int height, u32 color) {
  if (width <= 0 || height <= 0)
    return;

  const int screen_width = Multiboot2::framebuffer.width;
  const int screen_height = Multiboot2::framebuffer.height;

  const int start_x = x < 0 ? 0 : x;
  const int start_y = y < 0 ? 0 : y;

  const int end_x = x + width > screen_width ? screen_width : x + width;

  const int end_y = y + height > screen_height ? screen_height : y + height;

  for (int py = start_y; py < end_y; py++) {
    for (int px = start_x; px < end_x; px++)
      Graphics::put_pixel(px, py, color);
  }
}

inline void draw_line(u32 x, u32 y, u32 w, u32 color) {
  for (u32 i = x; i < x + w; i++) {
    put_pixel(i, y, color);
  }
}

inline void draw_line(int x1, int y1, int x2, int y2, u32 color) {
  int dx = MathUtils::abs(x2 - x1);
  int dy = MathUtils::abs(y2 - y1);

  int sx = x1 < x2 ? 1 : -1;
  int sy = y1 < y2 ? 1 : -1;

  int error = dx - dy;

  while (true) {
    put_pixel(x1, y1, color);

    if (x1 == x2 && y1 == y2)
      break;

    int error2 = error * 2;

    if (error2 > -dy) {
      error -= dy;
      x1 += sx;
    }

    if (error2 < dx) {
      error += dx;
      y1 += sy;
    }
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
  u32 offset_x = 0;
  u32 offset_y = 0;

  while (*str != '\0') {
    if (*str == '\n') {
      offset_x = 0;
      offset_y += CHARACTER_HEIGHT;
      str++;
      continue;
    }

    draw_char(*str, x + offset_x, y + offset_y, color);

    offset_x += CHARACTER_WIDTH;
    str++;
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

inline void draw_cursor(u32 x, u32 y) {
  draw_image(Cursor::get_current_cursor_bitmap()->pixels, x, y, Cursor::WIDTH,
             Cursor::HEIGHT);
}

inline void draw_vertical_line(u32 x, u32 y, u32 h, u32 color) {
  for (u32 i = y; i < y + h; ++i) {
    put_pixel(x, i, color);
  }
}

void draw_circle(int cx, int cy, int radius, u32 color) {
  int x = radius;
  int y = 0;
  int decision = 1 - radius;

  while (x >= y) {
    put_pixel(cx + x, cy + y, color);
    put_pixel(cx + y, cy + x, color);
    put_pixel(cx - y, cy + x, color);
    put_pixel(cx - x, cy + y, color);
    put_pixel(cx - x, cy - y, color);
    put_pixel(cx - y, cy - x, color);
    put_pixel(cx + y, cy - x, color);
    put_pixel(cx + x, cy - y, color);

    y++;

    if (decision <= 0) {
      decision += 2 * y + 1;
    } else {
      x--;
      decision += 2 * (y - x) + 1;
    }
  }
}

void draw_angled_line(int cx, int cy, int length, float angle, u32 color) {
  const int end_x = cx + static_cast<int>(MathUtils::cos(angle) * length);

  const int end_y = cy + static_cast<int>(MathUtils::sin(angle) * length);

  draw_line(cx, cy, end_x, end_y, color);
}

} // namespace Graphics
