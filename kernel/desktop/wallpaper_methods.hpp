#include "../../boot/graphics.hpp"
#include "../shared/types.hpp"

// ─── Unsigned-only thick line helper ───
static void draw_line(u32 x0, u32 y0, u32 x1, u32 y1, u32 color) {
  u32 dx = x0 > x1 ? x0 - x1 : x1 - x0;
  u32 dy = y0 > y1 ? y0 - y1 : y1 - y0;
  u32 steps = dx > dy ? dx : dy;
  if (steps == 0)
    steps = 1;

  for (u32 i = 0; i <= steps; i++) {
    u32 x = x0 <= x1 ? x0 + (dx * i) / steps : x0 - (dx * i) / steps;
    u32 y = y0 <= y1 ? y0 + (dy * i) / steps : y0 - (dy * i) / steps;

    Graphics::put_pixel(x, y, color);
    Graphics::put_pixel(x + 1, y, color);
    Graphics::put_pixel(x, y + 1, color);
  }
}

//  Industrial steel links on a dark forge floor.
static void draw_chains() {
  u32 w = Multiboot2::framebuffer.width;
  u32 h = Multiboot2::framebuffer.height;

  for (u32 y = 0; y < h; y++) {
    for (u32 x = 0; x < w; x++) {
      u32 t = (x + y) * 15 / (w + h);
      u32 r = 8 + t;
      u32 g = 8 + t;
      u32 b = 12 + t * 2;
      Graphics::put_pixel(x, y, (r << 16) | (g << 8) | b);
    }
  }

  u32 link_w = w / 8;
  u32 link_h = h / 6;
  u32 thick = link_w / 5;
  if (thick < 4)
    thick = 4;
  u32 rows = 3;
  u32 cols = 4;

  u32 steel_mid = 0x555555;
  u32 steel_light = 0x777777;

  for (u32 row = 0; row < rows; row++) {
    u32 y_off = h / 4 + row * (h / 4);
    for (u32 col = 0; col < cols; col++) {
      u32 x_off = w / 8 + col * (w / 5);
      u32 x_pos = x_off + ((row % 2) * (link_w / 2));

      Graphics::draw_rect(x_pos, y_off, link_w, link_h, steel_mid);
      Graphics::draw_rect(x_pos + thick, y_off + thick, link_w - thick * 2,
                          link_h - thick * 2, 0x050508);
      Graphics::draw_rect(x_pos, y_off, link_w, thick, steel_light);
    }
  }
}

//  Classic bone-white on a black void.
static void draw_skull() {
  u32 w = Multiboot2::framebuffer.width;
  u32 h = Multiboot2::framebuffer.height;

  for (u32 y = 0; y < h; y++) {
    u32 gray = y * 10 / h;
    u32 bg = (gray << 16) | (gray << 8) | gray;
    for (u32 x = 0; x < w; x++) {
      Graphics::put_pixel(x, y, bg);
    }
  }

  u32 cx = w / 2;
  u32 cy = h / 2;
  u32 size = w < h ? w / 3 : h / 3;

  u32 bone = 0xdddddd;
  u32 dark_bone = 0xaaaaaa;

  Graphics::draw_rect(cx - size, cy - size, size * 2, size, bone);
  Graphics::draw_rect(cx - size * 3 / 4, cy, size * 3 / 2, size * 3 / 4, bone);

  u32 eye_w = size / 2;
  u32 eye_h = size / 3;
  Graphics::draw_rect(cx - size * 2 / 3, cy - size / 4, eye_w, eye_h, 0x000000);
  Graphics::draw_rect(cx + size / 6, cy - size / 4, eye_w, eye_h, 0x000000);
  Graphics::draw_rect(cx - size / 8, cy + size / 8, size / 4, size / 6,
                      0x000000);

  u32 tooth_w = size / 8;
  u32 tooth_h = size / 6;
  for (u32 i = 0; i < 6; i++) {
    u32 tx = cx - size * 2 / 3 + i * (size * 4 / 6 / 6);
    Graphics::draw_rect(tx, cy + size / 2, tooth_w, tooth_h, dark_bone);
  }

  draw_line(cx, cy - size, cx - size / 3, cy - size / 2, 0x000000);
  draw_line(cx, cy - size, cx + size / 4, cy - size * 3 / 4, 0x000000);
}

//  Gothic purple void with a dark silhouette.
static void draw_bat_wings() {
  u32 w = Multiboot2::framebuffer.width;
  u32 h = Multiboot2::framebuffer.height;

  for (u32 y = 0; y < h; y++) {
    for (u32 x = 0; x < w; x++) {
      u32 t = y * 20 / h;
      Graphics::put_pixel(x, y, (5 << 16) | (2 << 8) | (8 + t));
    }
  }

  u32 cx = w / 2;
  u32 cy = h / 2;
  u32 wing_w = w / 3;
  u32 wing_h = h / 3;

  u32 wing_mid = 0x222222;
  u32 wing_dark = 0x111111;

  Graphics::draw_rect(cx - wing_w, cy - wing_h / 2, wing_w, wing_h / 4,
                      wing_mid);
  Graphics::draw_rect(cx - wing_w * 5 / 4, cy - wing_h / 4, wing_w * 5 / 4,
                      wing_h / 4, wing_mid);
  Graphics::draw_rect(cx - wing_w * 6 / 4, cy, wing_w * 6 / 4, wing_h / 4,
                      wing_mid);
  Graphics::draw_rect(cx - wing_w * 5 / 4, cy + wing_h / 4, wing_w * 5 / 4,
                      wing_h / 4, wing_dark);
  Graphics::draw_rect(cx - wing_w, cy + wing_h / 2, wing_w, wing_h / 4,
                      wing_dark);

  Graphics::draw_rect(cx, cy - wing_h / 2, wing_w, wing_h / 4, wing_mid);
  Graphics::draw_rect(cx, cy - wing_h / 4, wing_w * 5 / 4, wing_h / 4,
                      wing_mid);
  Graphics::draw_rect(cx, cy, wing_w * 6 / 4, wing_h / 4, wing_mid);
  Graphics::draw_rect(cx, cy + wing_h / 4, wing_w * 5 / 4, wing_h / 4,
                      wing_dark);
  Graphics::draw_rect(cx, cy + wing_h / 2, wing_w, wing_h / 4, wing_dark);

  Graphics::draw_rect(cx - wing_w / 6, cy - wing_h / 4, wing_w / 3, wing_h,
                      wing_mid);
  Graphics::draw_rect(cx - wing_w / 4, cy - wing_h / 2 - wing_h / 6, wing_w / 8,
                      wing_h / 6, wing_mid);
  Graphics::draw_rect(cx + wing_w / 8, cy - wing_h / 2 - wing_h / 6, wing_w / 8,
                      wing_h / 6, wing_mid);
}

//  Black void with a gray radial web.
static void draw_spider_web() {
  u32 w = Multiboot2::framebuffer.width;
  u32 h = Multiboot2::framebuffer.height;
  u32 cx = w / 2;
  u32 cy = h / 2;

  for (u32 y = 0; y < h; y++) {
    for (u32 x = 0; x < w; x++) {
      Graphics::put_pixel(x, y, (3 << 16) | (3 << 8) | 4);
    }
  }

  u32 web_color = 0x444444;
  u32 web_light = 0x666666;

  draw_line(cx, cy, w - 1, cy, web_color);
  draw_line(cx, cy, w - 1, 0, web_color);
  draw_line(cx, cy, cx, 0, web_color);
  draw_line(cx, cy, 0, 0, web_color);
  draw_line(cx, cy, 0, cy, web_color);
  draw_line(cx, cy, 0, h - 1, web_color);
  draw_line(cx, cy, cx, h - 1, web_color);
  draw_line(cx, cy, w - 1, h - 1, web_color);

  for (u32 ring = 1; ring <= 4; ring++) {
    u32 t = ring * 25;
    u32 p[8][2];

    p[0][0] = cx + ((w - 1 - cx) * t) / 100;
    p[0][1] = cy;
    p[1][0] = cx + ((w - 1 - cx) * t) / 100;
    p[1][1] = cy - (cy * t) / 100;
    p[2][0] = cx;
    p[2][1] = cy - (cy * t) / 100;
    p[3][0] = cx - (cx * t) / 100;
    p[3][1] = cy - (cy * t) / 100;
    p[4][0] = cx - (cx * t) / 100;
    p[4][1] = cy;
    p[5][0] = cx - (cx * t) / 100;
    p[5][1] = cy + ((h - 1 - cy) * t) / 100;
    p[6][0] = cx;
    p[6][1] = cy + ((h - 1 - cy) * t) / 100;
    p[7][0] = cx + ((w - 1 - cx) * t) / 100;
    p[7][1] = cy + ((h - 1 - cy) * t) / 100;

    for (u32 j = 0; j < 8; j++) {
      u32 next = (j + 1) % 8;
      draw_line(p[j][0], p[j][1], p[next][0], p[next][1], web_light);
    }
  }
}

//  Storm sky with electric bolts.
static void draw_lightning() {
  u32 w = Multiboot2::framebuffer.width;
  u32 h = Multiboot2::framebuffer.height;

  for (u32 y = 0; y < h; y++) {
    for (u32 x = 0; x < w; x++) {
      u32 t = y * 15 / h;
      Graphics::put_pixel(x, y, (2 << 16) | ((4 + t) << 8) | (12 + t * 2));
    }
  }

  u32 bolt_color = 0xffffaa;
  u32 bolt_core = 0xffffff;

  u32 x = w / 2;
  for (u32 y = 0; y < h; y += h / 20) {
    u32 next_y = y + h / 20;
    if (next_y > h)
      next_y = h;
    u32 offset = ((y * 7) % (w / 4));
    if (((y / (h / 20)) % 2) == 0) {
      x = w / 2 + offset;
    } else {
      x = w / 2 - offset + w / 8;
    }
    if (x > w - 20)
      x = w - 20;
    Graphics::draw_rect(x, y, 8, next_y - y, bolt_color);
    Graphics::draw_rect(x + 2, y, 4, next_y - y, bolt_core);
  }

  x = w / 3;
  for (u32 y = h / 6; y < h; y += h / 16) {
    u32 next_y = y + h / 16;
    if (next_y > h)
      next_y = h;
    u32 offset = ((y * 13) % (w / 6));
    if (((y / (h / 16)) % 2) == 0) {
      x = w / 3 + offset;
    } else {
      x = w / 3 - offset;
    }
    if (x > w - 10)
      x = w - 10;
    Graphics::draw_rect(x, y, 4, next_y - y, bolt_color);
  }
}

//  Blood-red void with a silver blade dripping.
static void draw_dagger() {
  u32 w = Multiboot2::framebuffer.width;
  u32 h = Multiboot2::framebuffer.height;

  for (u32 y = 0; y < h; y++) {
    for (u32 x = 0; x < w; x++) {
      u32 t = (x + y) * 20 / (w + h);
      u32 r = 20 - t;
      u32 b = 4;
      if (r > 20)
        r = 0;
      Graphics::put_pixel(x, y, (r << 16) | (2 << 8) | b);
    }
  }

  u32 cx = w / 2;
  u32 cy = h / 3;
  u32 blade_h = h / 2;
  u32 blade_w = w / 16;
  u32 handle_h = h / 8;
  u32 handle_w = w / 12;

  u32 silver = 0xcccccc;
  u32 silver_light = 0xeeeeee;
  u32 guard = 0x444444;
  u32 handle = 0x221111;
  u32 blood = 0xaa0000;

  for (u32 i = 0; i < blade_h / 4; i++) {
    u32 bw = blade_w - i / 2;
    if (bw < 2)
      bw = 2;
    Graphics::draw_rect(cx - bw / 2, cy + i * 4, bw, 4, silver);
  }
  Graphics::draw_rect(cx - blade_w / 4, cy, 2, blade_h, silver_light);

  Graphics::draw_rect(cx - handle_w, cy + blade_h, handle_w * 2, handle_h / 2,
                      guard);
  Graphics::draw_rect(cx - handle_w / 3, cy + blade_h + handle_h / 2,
                      handle_w * 2 / 3, handle_h, handle);
  Graphics::draw_rect(cx - handle_w / 2, cy + blade_h + handle_h * 3 / 2,
                      handle_w, handle_h / 2, guard);

  u32 drop_y = cy + blade_h;
  Graphics::draw_rect(cx - 2, drop_y, 4, 12, blood);
  Graphics::draw_rect(cx - 1, drop_y + 12, 2, 8, 0x880000);
}

//  Moldy green dark with iron-banded wood.
static void draw_coffin() {
  u32 w = Multiboot2::framebuffer.width;
  u32 h = Multiboot2::framebuffer.height;

  for (u32 y = 0; y < h; y++) {
    for (u32 x = 0; x < w; x++) {
      u32 t = y * 18 / h;
      u32 r = 4 + t / 2;
      u32 g = 8 + t;
      Graphics::put_pixel(x, y, (r << 16) | (g << 8) | 4);
    }
  }

  u32 cx = w / 2;
  u32 cy = h / 2;
  u32 cw = w / 5;
  u32 ch = h / 2;

  u32 wood_mid = 0x2a1a0e;
  u32 wood_dark = 0x1a1108;
  u32 iron = 0x333333;

  u32 top_w = cw * 3 / 2;
  u32 top_h = ch / 4;
  Graphics::draw_rect(cx - top_w / 2, cy - ch / 2, top_w, top_h, wood_mid);

  u32 mid_w = cw;
  u32 mid_h = ch / 2;
  Graphics::draw_rect(cx - mid_w / 2, cy - ch / 2 + top_h, mid_w, mid_h,
                      wood_dark);

  u32 bot_w = cw * 2 / 3;
  u32 bot_h = ch / 4;
  Graphics::draw_rect(cx - bot_w / 2, cy + ch / 2 - bot_h, bot_w, bot_h,
                      wood_mid);

  Graphics::draw_rect(cx - top_w / 2 - 2, cy - ch / 4, top_w + 4, 4, iron);
  Graphics::draw_rect(cx - mid_w / 2 - 2, cy, mid_w + 4, 4, iron);
  Graphics::draw_rect(cx - bot_w / 2 - 2, cy + ch / 4, bot_w + 4, 4, iron);

  u32 plate_w = cw / 2;
  u32 plate_h = ch / 10;
  Graphics::draw_rect(cx - plate_w / 2, cy - ch / 8, plate_w, plate_h, iron);
}

//  Black to deep red with jagged flame tongues.
static void draw_inferno() {
  u32 w = Multiboot2::framebuffer.width;
  u32 h = Multiboot2::framebuffer.height;

  for (u32 y = 0; y < h; y++) {
    for (u32 x = 0; x < w; x++) {
      u32 t = y * 30 / h;
      u32 g = t / 4;
      Graphics::put_pixel(x, y, (t << 16) | (g << 8) | 2);
    }
  }

  for (u32 f = 0; f < 7; f++) {
    u32 fx = w / 8 + f * (w * 6 / 8 / 7);
    u32 fheight = h / 4 + ((f * 137 + w) % (h / 3));
    u32 fwidth = w / 20 + ((f * 53) % (w / 15));

    u32 fg = 0x44 + (f * 20);
    if (fg > 0xaa)
      fg = 0xaa;
    u32 flame_color = (0xff << 16) | (fg << 8);

    for (u32 i = 0; i < fheight; i += 4) {
      u32 fw = fwidth - (i * fwidth) / (fheight * 2);
      if (fw < 2)
        fw = 2;
      u32 offset = ((fx + i * 7) % 8);
      Graphics::draw_rect(fx + offset, h - 1 - i - 4, fw, 4, flame_color);
    }
  }
}

//  Dark blue void with a crimson moon and craters.
static void draw_blood_moon() {
  u32 w = Multiboot2::framebuffer.width;
  u32 h = Multiboot2::framebuffer.height;

  for (u32 y = 0; y < h; y++) {
    for (u32 x = 0; x < w; x++) {
      u32 b = 6 + (y * 4 / h);
      Graphics::put_pixel(x, y, (2 << 16) | (3 << 8) | b);
    }
  }

  u32 cx = w / 2;
  u32 cy = h / 2;
  u32 R = w < h ? w / 3 : h / 3;

  u32 moon_dark = 0x550000;
  u32 moon_mid = 0x880000;
  u32 moon_light = 0xbb1111;
  u32 moon_bright = 0xdd2222;

  for (u32 y = cy - R; y <= cy + R; y++) {
    if (y >= h)
      break;
    for (u32 x = cx - R; x <= cx + R; x++) {
      if (x >= w)
        break;
      u32 dx = x > cx ? x - cx : cx - x;
      u32 dy = y > cy ? y - cy : cy - y;
      if (dx * dx + dy * dy <= R * R) {
        u32 c1dx = x > cx - R / 3 ? x - (cx - R / 3) : (cx - R / 3) - x;
        u32 c1dy = y > cy - R / 3 ? y - (cy - R / 3) : (cy - R / 3) - y;
        u32 c2dx = x > cx + R / 4 ? x - (cx + R / 4) : (cx + R / 4) - x;
        u32 c2dy = y > cy + R / 5 ? y - (cy + R / 5) : (cy + R / 5) - y;

        u32 color = moon_mid;
        if (dx * dx + dy * dy <= (R * 3 / 4) * (R * 3 / 4))
          color = moon_light;
        if (c1dx * c1dx + c1dy * c1dy <= (R / 5) * (R / 5))
          color = moon_dark;
        if (c2dx * c2dx + c2dy * c2dy <= (R / 6) * (R / 6))
          color = moon_dark;
        if (dx * dx + dy * dy <= (R / 3) * (R / 3))
          color = moon_bright;

        Graphics::put_pixel(x, y, color);
      }
    }
  }

  u32 drip_y = cy + R * 3 / 4;
  Graphics::draw_rect(cx - 2, drip_y, 4, 12, moon_bright);
  Graphics::draw_rect(cx - 1, drip_y + 12, 2, 8, moon_mid);
}

//  Dark purple void with a shattered crimson heart.
static void draw_broken_heart() {
  u32 w = Multiboot2::framebuffer.width;
  u32 h = Multiboot2::framebuffer.height;

  for (u32 y = 0; y < h; y++) {
    for (u32 x = 0; x < w; x++) {
      u32 t = (x * 10 / w) + (y * 5 / h);
      u32 r = 8 + t;
      u32 b = 12 + t;
      Graphics::put_pixel(x, y, (r << 16) | (2 << 8) | b);
    }
  }

  u32 cx = w / 2;
  u32 cy = h / 2;
  u32 size = w < h ? w / 4 : h / 4;

  u32 heart_mid = 0x66001a;
  u32 heart_dark = 0x440011;
  u32 heart_bright = 0x880022;
  u32 crack = 0x000000;

  Graphics::draw_rect(cx - size, cy - size, size, size, heart_mid);
  Graphics::draw_rect(cx - size * 3 / 4, cy, size * 3 / 4, size, heart_dark);
  Graphics::draw_rect(cx - size / 2, cy + size, size / 2, size / 2, heart_dark);

  Graphics::draw_rect(cx, cy - size, size, size, heart_mid);
  Graphics::draw_rect(cx, cy, size * 3 / 4, size, heart_dark);
  Graphics::draw_rect(cx, cy + size, size / 2, size / 2, heart_dark);

  Graphics::draw_rect(cx - size / 4, cy - size / 2, size / 2, size / 3,
                      heart_bright);

  u32 crack_x = cx;
  for (u32 i = 0; i < size * 2; i += 4) {
    u32 offset = (i % 12) / 4;
    if ((i / 4) % 2 == 0) {
      crack_x = cx + offset;
    } else {
      crack_x = cx - 1 - offset;
    }
    if (crack_x >= w)
      crack_x = w - 1;
    Graphics::draw_rect(crack_x, cy - size / 2 + i, 2, 4, crack);
  }
}
