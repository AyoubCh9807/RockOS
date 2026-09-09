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

// ─── Unsigned-only thick line helper (same trick as yours) ───
static void rock_line(u32 x0, u32 y0, u32 x1, u32 y1, u32 color) {
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

//  Amp stack with glowing tubes on a smoky stage.
static void draw_amp_stack() {
  u32 w = Multiboot2::framebuffer.width;
  u32 h = Multiboot2::framebuffer.height;

  for (u32 y = 0; y < h; y++) {
    for (u32 x = 0; x < w; x++) {
      u32 t = y * 24 / h;
      u32 g = 4 + t / 3;
      Graphics::put_pixel(x, y, (g << 16) | (g << 8) | (6 + t));
    }
  }

  u32 amp_w = w / 2;
  u32 amp_h = h / 3;
  u32 ax = (w - amp_w) / 2;
  u32 head_y = h / 5;

  // head unit
  Graphics::draw_rect(ax, head_y, amp_w, amp_h / 3, 0x111111);
  Graphics::draw_rect(ax, head_y, amp_w, 3, 0x333333);
  for (u32 i = 0; i < 6; i++) {
    u32 kx = ax + amp_w / 8 + i * (amp_w * 3 / 4 / 6);
    u32 ky = head_y + amp_h / 12;
    Graphics::draw_rect(kx, ky, 8, 8, 0xdddddd);
    Graphics::draw_rect(kx + 3, ky, 2, 3, 0x111111);
  }
  Graphics::draw_rect(ax + amp_w - 24, head_y + amp_h / 12, 6, 6, 0xff2222);

  // cabinet with grille cloth
  u32 cab_y = head_y + amp_h / 3 + 4;
  Graphics::draw_rect(ax, cab_y, amp_w, amp_h, 0x181818);
  Graphics::draw_rect(ax, cab_y, amp_w, 3, 0x333333);
  for (u32 gy = cab_y + 8; gy + 8 < cab_y + amp_h; gy += 6) {
    for (u32 gx = ax + 8; gx + 8 < ax + amp_w; gx += 6) {
      Graphics::put_pixel(gx, gy, 0x2a2a2a);
    }
  }

  // two glowing speakers
  for (u32 s = 0; s < 2; s++) {
    u32 sx = ax + amp_w / 4 + s * (amp_w / 2);
    u32 sy = cab_y + amp_h / 2;
    u32 R = amp_h / 3;
    for (u32 y = 0; y < h; y++) {
      for (u32 x = ax; x < ax + amp_w; x++) {
        u32 dx = x > sx ? x - sx : sx - x;
        u32 dy = y > sy ? y - sy : sy - y;
        u32 d2 = dx * dx + dy * dy;
        if (d2 <= R * R) {
          u32 g = 0x22 + ((R * R - d2) * 0x55) / (R * R);
          Graphics::put_pixel(x, y, (g << 16) | (g << 8) | g);
        }
      }
    }
  }

  // glowing tubes along the top
  for (u32 i = 0; i < 5; i++) {
    u32 tx = w / 4 + i * (w / 2 / 5);
    u32 ty = h / 14;
    Graphics::draw_rect(tx, ty, 12, 20, 0x331008);
    Graphics::draw_rect(tx + 3, ty + 4, 6, 12, 0xff7722);
    Graphics::draw_rect(tx + 5, ty + 6, 2, 8, 0xffcc66);
  }
}

//  Neon green-to-red equalizer bars with a floor reflection.
static void draw_equalizer() {
  u32 w = Multiboot2::framebuffer.width;
  u32 h = Multiboot2::framebuffer.height;

  for (u32 y = 0; y < h; y++) {
    for (u32 x = 0; x < w; x++) {
      Graphics::put_pixel(x, y, 0x000000);
    }
  }

  u32 bars = 28;
  for (u32 b = 0; b < bars; b++) {
    u32 bh = h / 8 + ((b * 137 + w * 3) % (h * 3 / 4));
    u32 bx = b * w / bars + 2;
    u32 bw = w / bars - 4;
    if (bw < 2)
      bw = 2;

    for (u32 y = 0; y < bh && y < h; y += 2) {
      u32 t = y * 255 / h;
      u32 r = t;
      u32 g = t < 100 ? 200 + t / 2 : 255 - (t - 100);
      u32 color = (r << 16) | (g << 8) | 0x22;
      Graphics::draw_rect(bx, h - 1 - y, bw, 1, color);
      if (h - 1 - y + 2 < h)
        Graphics::draw_rect(bx, h - 1 - y + 2, bw, 1, color & 0x3f3f3f);
    }
  }
}

//  Red electric guitar under a spotlight.
static void draw_guitar() {
  u32 w = Multiboot2::framebuffer.width;
  u32 h = Multiboot2::framebuffer.height;
  u32 cx = w / 2;
  u32 cy = h * 2 / 3;

  for (u32 y = 0; y < h; y++) {
    for (u32 x = 0; x < w; x++) {
      u32 dx = x > cx ? x - cx : cx - x;
      u32 t = 0;
      if (dx < w / 3)
        t = (w / 3 - dx) * 30 / (w / 3);
      u32 g = 4 + t / 2;
      Graphics::put_pixel(x, y, (g << 16) | (g << 8) | (6 + t));
    }
  }

  u32 bw = w / 6;
  u32 bh = h / 4;

  Graphics::draw_rect(cx - bw, cy - bh / 2, bw * 2, bh, 0x881111);
  Graphics::draw_rect(cx - bw + 6, cy - bh / 2 + 6, bw * 2 - 12, bh - 12,
                      0xaa2222);
  Graphics::draw_rect(cx - bw / 2, cy - bh / 2 - 8, bw, 8, 0xaa2222);
  Graphics::draw_rect(cx, cy - bh / 4, bw / 2, bh / 2, 0xcccccc);
  Graphics::draw_rect(cx + bw / 4, cy - bh / 4, 4, bh / 2, 0x333333);

  u32 neck_w = w / 24;
  u32 neck_top = h / 12;
  u32 neck_bottom = cy - bh / 2;
  Graphics::draw_rect(cx - neck_w / 2, neck_top, neck_w, neck_bottom - neck_top,
                      0x553311);
  for (u32 fy = neck_top + 10; fy < neck_bottom; fy += 14) {
    for (u32 x = cx - neck_w / 2; x < cx + neck_w / 2; x++) {
      Graphics::put_pixel(x, fy, 0xaaaaaa);
    }
  }
  Graphics::draw_rect(cx - neck_w, neck_top - h / 24, neck_w * 2, h / 24,
                      0x332211);
  for (u32 s = 0; s < 5; s++) {
    u32 x0 = cx - neck_w / 2 + s * (neck_w / 4);
    u32 x1 = cx - bw / 2 + s * (bw / 4);
    rock_line(x0, neck_top, x1, cy + bh / 3, 0xdddddd);
  }
}

//  Skull wearing glowing headphones on a black void.
static void draw_headphone_skull() {
  u32 w = Multiboot2::framebuffer.width;
  u32 h = Multiboot2::framebuffer.height;
  u32 cx = w / 2;
  u32 cy = h / 2;
  u32 size = (w < h ? w : h) / 3;

  for (u32 y = 0; y < h; y++) {
    u32 gray = y * 10 / h;
    u32 bg = (gray << 16) | (gray << 8) | gray;
    for (u32 x = 0; x < w; x++) {
      Graphics::put_pixel(x, y, bg);
    }
  }

  u32 bone = 0xdddddd;
  u32 dark_bone = 0xaaaaaa;

  Graphics::draw_rect(cx - size, cy - size, size * 2, size, bone);
  Graphics::draw_rect(cx - size * 3 / 4, cy, size * 3 / 2, size * 3 / 4, bone);

  u32 eye_w = size / 2;
  u32 eye_h = size / 3;
  Graphics::draw_rect(cx - size * 2 / 3, cy - size / 4, eye_w, eye_h, 0x000000);
  Graphics::draw_rect(cx + size / 6, cy - size / 4, eye_w, eye_h, 0x000000);
  Graphics::draw_rect(cx - size / 8, cy + size / 8, size / 4, size / 6, 0x000000);

  u32 tooth_w = size / 8;
  u32 tooth_h = size / 6;
  for (u32 i = 0; i < 6; i++) {
    u32 tx = cx - size * 2 / 3 + i * (size * 4 / 6 / 6);
    Graphics::draw_rect(tx, cy + size / 2, tooth_w, tooth_h, dark_bone);
  }

  // headphone band: ring arc over the top half of the skull
  u32 Rb = size + 6;
  for (u32 y = cy - Rb - 10; y < cy; y++) {
    if (y >= h)
      break;
    for (u32 x = cx - Rb - 10; x < cx + Rb + 10; x++) {
      if (x >= w)
        break;
      u32 dx = x > cx ? x - cx : cx - x;
      u32 dy = y > cy ? y - cy : cy - y;
      u32 d2 = dx * dx + dy * dy;
      if (d2 <= Rb * Rb && d2 >= (Rb - 6) * (Rb - 6)) {
        Graphics::put_pixel(x, y, 0x888888);
      }
      if (d2 <= (Rb + 2) * (Rb + 2) && d2 >= Rb * Rb) {
        Graphics::put_pixel(x, y, 0x22ccff);
      }
    }
  }

  // ear cups with cyan glow
  u32 cup_w = 14;
  u32 cup_h = size / 2 + 10;
  Graphics::draw_rect(cx - Rb - cup_w + 2, cy - cup_h / 2, cup_w, cup_h, 0x111111);
  Graphics::draw_rect(cx + Rb - 2, cy - cup_h / 2, cup_w, cup_h, 0x111111);
  Graphics::draw_rect(cx - Rb - cup_w + 2, cy - cup_h / 2, 3, cup_h, 0x22ccff);
  Graphics::draw_rect(cx + Rb + cup_w - 5, cy - cup_h / 2, 3, cup_h, 0x22ccff);
}

//  Spinning-look vinyl record on a black void.
static void draw_vinyl() {
  u32 w = Multiboot2::framebuffer.width;
  u32 h = Multiboot2::framebuffer.height;
  u32 cx = w / 2;
  u32 cy = h / 2;
  u32 R = (w < h ? w : h) / 3;

  for (u32 y = 0; y < h; y++) {
    for (u32 x = 0; x < w; x++) {
      Graphics::put_pixel(x, y, 0x050505);
    }
  }

  for (u32 y = cy > R ? cy - R : 0; y <= cy + R && y < h; y++) {
    for (u32 x = cx > R ? cx - R : 0; x <= cx + R && x < w; x++) {
      u32 dx = x > cx ? x - cx : cx - x;
      u32 dy = y > cy ? y - cy : cy - y;
      u32 d2 = dx * dx + dy * dy;
      if (d2 > R * R)
        continue;

      u32 d = 0;
      while (d * d < d2)
        d++;

      if (d < R / 4) {
        Graphics::put_pixel(x, y, 0xaa1111);
      } else if (d < R / 4 + 2) {
        Graphics::put_pixel(x, y, 0xdd2222);
      } else if (d % 7 < 2) {
        Graphics::put_pixel(x, y, 0x333333);
      } else {
        Graphics::put_pixel(x, y, 0x101010);
      }
    }
  }

  // sheen
  for (u32 y = cy > R ? cy - R : 0; y <= cy + R && y < h; y++) {
    for (u32 x = cx > R ? cx - R : 0; x <= cx + R && x < w; x++) {
      if (y < cy - R / 3 && x < cx - R / 4) {
        u32 dx = x > cx ? x - cx : cx - x;
        u32 dy = y > cy ? y - cy : cy - y;
        if (dx * dx + dy * dy <= R * R)
          Graphics::put_pixel(x, y, 0x3a3a3a);
      }
    }
  }
  Graphics::draw_rect(cx - 2, cy - 2, 4, 4, 0x111111);
}

//  Wall of speaker cabinets, some cones glowing.
static void draw_speaker_wall() {
  u32 w = Multiboot2::framebuffer.width;
  u32 h = Multiboot2::framebuffer.height;

  for (u32 y = 0; y < h; y++) {
    for (u32 x = 0; x < w; x++) {
      Graphics::put_pixel(x, y, (6 << 16) | (6 << 8) | 8);
    }
  }

  u32 cols = 6;
  u32 rows = 3;
  u32 cw = w / cols;
  u32 ch = h / rows;

  for (u32 r = 0; r < rows; r++) {
    for (u32 c = 0; c < cols; c++) {
      u32 bx = c * cw + 4;
      u32 by = r * ch + 4;
      u32 bw = cw - 8;
      u32 bh = ch - 8;
      Graphics::draw_rect(bx, by, bw, bh, 0x141414);
      Graphics::draw_rect(bx, by, bw, 2, 0x2a2a2a);

      u32 sx = bx + bw / 2;
      u32 sy = by + bh / 2;
      u32 R = (bw < bh ? bw : bh) / 3;
      u32 glow = (r * cols + c) % 3;

      for (u32 y = by; y < by + bh; y++) {
        for (u32 x = bx; x < bx + bw; x++) {
          u32 dx = x > sx ? x - sx : sx - x;
          u32 dy = y > sy ? y - sy : sy - y;
          u32 d2 = dx * dx + dy * dy;
          if (d2 <= R * R) {
            u32 g = 0x1a + ((R * R - d2) * (glow ? 0x60 : 0x28)) / (R * R);
            u32 color = (g << 16) | (g << 8) | g;
            if (glow == 1)
              color = (g << 16) | (g << 8) | (g > 0x80 ? 0x80 : g);
            Graphics::put_pixel(x, y, color);
          }
        }
      }
    }
  }
}

//  Mosh pit: crowd silhouettes throwing horns under a stage light.
static void draw_moshpit() {
  u32 w = Multiboot2::framebuffer.width;
  u32 h = Multiboot2::framebuffer.height;
  u32 cx = w / 2;

  for (u32 y = 0; y < h; y++) {
    for (u32 x = 0; x < w; x++) {
      u32 t = y * 20 / h;
      Graphics::put_pixel(x, y, ((6 + t / 2) << 16) | (4 << 8) | (8 + t));
    }
  }

  // light cone from the top
  for (u32 y = 0; y < h; y++) {
    u32 hw = w / 10 + (y * 2) / 5;
    u32 t = 40 - (y * 30) / h;
    for (u32 x = cx > hw ? cx - hw : 0; x < cx + hw && x < w; x++) {
      if ((x + y) % 2 == 0)
        continue;
      u32 v = t > 0x28 ? t : 0x28;
      Graphics::put_pixel(x, y, (v << 16) | (v << 8) | v);
    }
  }

  // crowd
  u32 people = w / 26;
  for (u32 i = 0; i < people; i++) {
    u32 px = 10 + i * (w - 20) / people;
    u32 py = h - h / 8 - ((i * 37) % (h / 5));
    u32 R = h / 28;

    for (u32 y = py > R ? py - R : 0; y <= py + R && y < h; y++) {
      for (u32 x = px > R ? px - R : 0; x <= px + R && x < w; x++) {
        u32 dx = x > px ? x - px : px - x;
        u32 dy = y > py ? y - py : py - y;
        if (dx * dx + dy * dy <= R * R)
          Graphics::put_pixel(x, y, 0x050508);
      }
    }

    // raised arm
    u32 arm_top = py - R - h / 7 - ((i * 53) % (h / 10));
    rock_line(px, py - R, px + ((i % 3) * 4) - 4, arm_top, 0x050508);
    // horns: two prongs
    u32 hx = px + ((i % 3) * 4) - 4;
    rock_line(hx, arm_top, hx - 4, arm_top - 8, 0x050508);
    rock_line(hx, arm_top, hx + 4, arm_top - 8, 0x050508);
  }
}

//  Black venue with colored spotlight beams.
static void draw_stage_lights() {
  u32 w = Multiboot2::framebuffer.width;
  u32 h = Multiboot2::framebuffer.height;

  for (u32 y = 0; y < h; y++) {
    for (u32 x = 0; x < w; x++) {
      Graphics::put_pixel(x, y, 0x010101);
    }
  }
  for (u32 x = 0; x < w; x++) {
    Graphics::put_pixel(x, h - 2, 0x111111);
    Graphics::put_pixel(x, h - 1, 0x0a0a0a);
  }

  static const u32 beam_colors[5] = {0x22ccff, 0xcc44ff, 0xffcc33, 0x33ff88,
                                     0xff4444};

  for (u32 i = 0; i < 5; i++) {
    u32 sx = w / 6 + i * (w * 4 / 6 / 4 == 0 ? 1 : w * 4 / 6 / 4);
    sx = w / 6 + i * (w / 6);
    int dir = (int)i - 2;
    for (u32 t = 0; t < h; t += 2) {
      u32 hw = 6 + t / 5;
      int center = (int)sx + (dir * (int)t) / 4;
      u32 bright = 0xff - (t * 0xc0) / h;
      u32 base = beam_colors[i];
      u32 r = (((base >> 16) & 0xff) * bright) / 0xff;
      u32 g = (((base >> 8) & 0xff) * bright) / 0xff;
      u32 b = ((base & 0xff) * bright) / 0xff;
      u32 color = (r << 16) | (g << 8) | b;
      for (int d = -(int)hw; d <= (int)hw; d += 2) {
        int x = center + d;
        if (x >= 0 && (u32)x < w)
          Graphics::put_pixel((u32)x, t, color);
      }
    }
    Graphics::draw_rect(sx - 4, 0, 10, 8, 0x222222);
  }
}

//  Raven on a branch under a pale moon.
static void draw_raven() {
  u32 w = Multiboot2::framebuffer.width;
  u32 h = Multiboot2::framebuffer.height;

  for (u32 y = 0; y < h; y++) {
    for (u32 x = 0; x < w; x++) {
      u32 t = y * 14 / h;
      u32 r = 18 + t;
      u32 b = 28 + t;
      Graphics::put_pixel(x, y, (r << 16) | (r / 2 << 8) | b);
    }
  }

  // moon
  u32 mx = w / 5;
  u32 my = h / 5;
  u32 mr = (w < h ? w : h) / 10;
  for (u32 y = my > mr ? my - mr : 0; y <= my + mr && y < h; y++) {
    for (u32 x = mx > mr ? mx - mr : 0; x <= mx + mr && x < w; x++) {
      u32 dx = x > mx ? x - mx : mx - x;
      u32 dy = y > my ? y - my : my - y;
      if (dx * dx + dy * dy <= mr * mr)
        Graphics::put_pixel(x, y, 0xcfd4dd);
    }
  }

  // branch
  rock_line(0, h * 3 / 4, w, h * 3 / 4 + h / 16, 0x111111);
  for (u32 i = 1; i < 5; i++) {
    u32 bx = i * w / 5;
    u32 by = h * 3 / 4 + (bx * (h / 16)) / w;
    rock_line(bx, by, bx + w / 20, by - h / 12, 0x111111);
  }

  // raven silhouette
  u32 rx = w * 3 / 5;
  u32 ry = h * 3 / 4 - h / 12;
  u32 R1 = h / 16;
  u32 body_color = 0x0a0a10;

  for (u32 y = ry > R1 ? ry - R1 : 0; y <= ry + R1 && y < h; y++) {
    for (u32 x = rx > R1 ? rx - R1 : 0; x <= rx + R1 && x < w; x++) {
      u32 dx = (x > rx ? x - rx : rx - x) * 3 / 4;
      u32 dy = y > ry ? y - ry : ry - y;
      if (dx * dx + dy * dy <= R1 * R1)
        Graphics::put_pixel(x, y, body_color);
    }
  }

  u32 hx = rx + R1;
  u32 hy = ry - R1;
  u32 R2 = R1 * 3 / 4;
  for (u32 y = hy > R2 ? hy - R2 : 0; y <= hy + R2 && y < h; y++) {
    for (u32 x = hx > R2 ? hx - R2 : 0; x <= hx + R2 && x < w; x++) {
      u32 dx = x > hx ? x - hx : hx - x;
      u32 dy = y > hy ? y - hy : hy - y;
      if (dx * dx + dy * dy <= R2 * R2)
        Graphics::put_pixel(x, y, body_color);
    }
  }

  // beak + eye + tail
  Graphics::draw_rect(hx + R2 - 2, hy - 2, R2 + 6, 4, body_color);
  Graphics::put_pixel(hx + R2 / 2, hy - 1, 0xff2222);
  Graphics::draw_rect(rx - R1 - 14, ry + R1 / 2, 16, 4, body_color);
  Graphics::draw_rect(rx - R1 - 18, ry + R1 / 2 + 4, 18, 4, body_color);
}

//  Gray void crossed by barbed wire.
static void draw_barbed_wire() {
  u32 w = Multiboot2::framebuffer.width;
  u32 h = Multiboot2::framebuffer.height;

  for (u32 y = 0; y < h; y++) {
    for (u32 x = 0; x < w; x++) {
      u32 g = 24 + ((x + y) * 36) / (w + h);
      Graphics::put_pixel(x, y, (g << 16) | (g << 8) | (g + 4));
    }
  }

  // two parallel diagonal strands
  u32 y1a = h / 3, y1b = h / 2;
  u32 y2a = h * 2 / 3, y2b = h / 2 + h / 6;
  rock_line(0, y1a, w - 1, y1b, 0x888888);
  rock_line(0, y2a, w - 1, y2b < h ? y2b : h - 1, 0x888888);

  // barbs
  for (u32 i = 1; i < 16; i++) {
    u32 x = i * w / 16;
    u32 top = y1a + (x * (y1b - y1a)) / w;
    u32 bot = y2a + (x * ((y2b < h ? y2b : h - 1) - y2a)) / w;
    if (bot >= h)
      bot = h - 1;
    if (top >= bot)
      continue;
    rock_line(x, top, x, bot, 0x666666);
    // spikes at both ends
    rock_line(x, top, x - 5, top - 5, 0xaaaaaa);
    rock_line(x, top, x + 5, top - 5, 0xaaaaaa);
    rock_line(x, bot, x - 5, bot + 5 < h ? bot + 5 : h - 1, 0xaaaaaa);
    rock_line(x, bot, x + 5, bot + 5 < h ? bot + 5 : h - 1, 0xaaaaaa);
  }
}

//  Gothic castle silhouette under a cratered moon.
static void draw_dark_castle() {
  u32 w = Multiboot2::framebuffer.width;
  u32 h = Multiboot2::framebuffer.height;

  for (u32 y = 0; y < h; y++) {
    for (u32 x = 0; x < w; x++) {
      u32 b = 10 + (y * 6) / h;
      Graphics::put_pixel(x, y, (2 << 16) | (3 << 8) | b);
      if (((x * 73 + y * 151) % 977) == 0 && y < h / 2) {
        Graphics::put_pixel(x, y, 0x888888);
      }
    }
  }

  u32 mx = w * 4 / 5;
  u32 my = h / 5;
  u32 mr = (w < h ? w : h) / 12;
  for (u32 y = my > mr ? my - mr : 0; y <= my + mr && y < h; y++) {
    for (u32 x = mx > mr ? mx - mr : 0; x <= mx + mr && x < w; x++) {
      u32 dx = x > mx ? x - mx : mx - x;
      u32 dy = y > my ? y - my : my - y;
      if (dx * dx + dy * dy <= mr * mr) {
        u32 g = 0xd0 - ((dx + dy) * 0x20) / mr;
        Graphics::put_pixel(x, y, (g << 16) | (g << 8) | g);
      }
    }
  }

  // ground
  for (u32 y = h * 2 / 3; y < h; y++) {
    for (u32 x = 0; x < w; x++) {
      Graphics::put_pixel(x, y, 0x050508);
    }
  }

  u32 stone = 0x0d0d16;
  u32 base_y = h * 2 / 3;
  u32 base_h = h / 6;

  // main keep
  Graphics::draw_rect(w / 3, base_y - h / 8, w / 3, h / 8 + base_h, stone);

  // towers
  u32 tw = w / 10;
  u32 tower_xs[3] = {w / 4, w / 2 - tw / 2, w * 3 / 4 - tw};
  u32 tower_h[3] = {h / 3, h / 2 - h / 12, h / 3 + h / 16};
  for (u32 i = 0; i < 3; i++) {
    u32 tx = tower_xs[i];
    u32 th = tower_h[i];
    u32 top = base_y + base_h - th;
    Graphics::draw_rect(tx, top, tw, th, stone);
    // crenellations
    for (u32 c = 0; c < 4; c++) {
      Graphics::draw_rect(tx + c * (tw / 4), top - 6, tw / 8, 6, stone);
    }
    // cone roof (stacked shrinking rects)
    for (u32 r = 0; r < h / 10; r += 4) {
      u32 rw = (tw / 2) * (h / 10 - r) / (h / 10);
      if (rw < 2)
        rw = 2;
      Graphics::draw_rect(tx + tw / 2 - rw / 2, top - 6 - h / 10 + r, rw, 4,
                          0x2a0808);
    }
    // lit windows
    Graphics::draw_rect(tx + tw / 2 - 3, top + th / 3, 6, 10, 0xffcc55);
    Graphics::draw_rect(tx + tw / 2 - 3, top + th * 2 / 3, 6, 10, 0xcc9933);
  }

  // keep windows
  for (u32 i = 0; i < 3; i++) {
    Graphics::draw_rect(w / 2 - 24 + i * 18, base_y - h / 16, 6, 10,
                        i == 1 ? 0xffcc55 : 0x996622);
  }
}

//  Drum kit waiting on a dark stage.
static void draw_drum_kit() {
  u32 w = Multiboot2::framebuffer.width;
  u32 h = Multiboot2::framebuffer.height;
  u32 cx = w / 2;
  u32 base = h * 3 / 5;

  for (u32 y = 0; y < h; y++) {
    for (u32 x = 0; x < w; x++) {
      u32 t = y * 16 / h;
      Graphics::put_pixel(x, y, ((8 + t / 2) << 16) | ((4 + t / 3) << 8) | (6 + t));
    }
  }

  auto circle_ring = [&](u32 ccx, u32 ccy, u32 R, u32 rim, u32 inner) {
    for (u32 y = ccy > R ? ccy - R : 0; y <= ccy + R && y < h; y++) {
      for (u32 x = ccx > R ? ccx - R : 0; x <= ccx + R && x < w; x++) {
        u32 dx = x > ccx ? x - ccx : ccx - x;
        u32 dy = y > ccy ? y - ccy : ccy - y;
        u32 d2 = dx * dx + dy * dy;
        if (d2 <= R * R)
          Graphics::put_pixel(x, y, d2 >= (R - 4) * (R - 4) ? rim : inner);
      }
    }
  };

  u32 R = h / 6;
  circle_ring(cx, base, R, 0xcccccc, 0x181818);
  Graphics::draw_rect(cx - 10, base - 6, 20, 12, 0x222222);

  circle_ring(cx - R - R / 2, base - R - h / 12, R / 2, 0xaaaaaa, 0x881111);
  circle_ring(cx + R + R / 2, base - R - h / 12, R / 2, 0xaaaaaa, 0x881111);
  circle_ring(cx - 2 * R - R / 3, base, R / 3, 0xbbbbbb, 0x999999);

  // cymbals
  u32 cym_y1 = base - R - h / 6;
  u32 cym1_x = cx + 2 * R + R / 2;
  for (u32 x = cym1_x - w / 14; x < cym1_x + w / 14 && x < w; x++) {
    Graphics::put_pixel(x, cym_y1, 0xddcc66);
    Graphics::put_pixel(x, cym_y1 + 1, 0xbbaa44);
  }
  rock_line(cym1_x, cym_y1 + 2, cym1_x, base + h / 8, 0x555555);
  Graphics::draw_rect(cym1_x - 8, base + h / 8, 16, 3, 0x555555);

  u32 cym2_x = cx - 2 * R - R / 2;
  u32 cym_y2 = base - R / 2;
  for (u32 x = cym2_x > w / 16 ? cym2_x - w / 16 : 0; x < cym2_x + w / 16; x++) {
    Graphics::put_pixel(x, cym_y2, 0xddcc66);
    Graphics::put_pixel(x, cym_y2 + 1, 0xbbaa44);
  }
  rock_line(cym2_x, cym_y2 + 2, cym2_x, base + h / 8, 0x555555);
  Graphics::draw_rect(cym2_x - 8, base + h / 8, 16, 3, 0x555555);
}

//  Microphone in a spotlight, cable trailing off.
static void draw_mic_stand() {
  u32 w = Multiboot2::framebuffer.width;
  u32 h = Multiboot2::framebuffer.height;
  u32 cx = w / 2;

  for (u32 y = 0; y < h; y++) {
    for (u32 x = 0; x < w; x++) {
      Graphics::put_pixel(x, y, 0x020202);
    }
  }

  for (u32 y = 0; y < h; y++) {
    u32 hw = w / 10 + (y * 3) / 8;
    u32 v = 0x30 - (y * 0x18) / h;
    for (u32 x = cx > hw ? cx - hw : 0; x < cx + hw && x < w; x++) {
      if ((x * 3 + y) % 4 == 0)
        continue;
      u32 g = v > 0x10 ? v : 0x10;
      Graphics::put_pixel(x, y, (g << 16) | (g << 8) | ((g * 3) / 4));
    }
  }

  u32 my = h / 3;
  u32 R = h / 26;

  // mic head
  for (u32 y = my > R ? my - R : 0; y <= my + R && y < h; y++) {
    for (u32 x = cx > R ? cx - R : 0; x <= cx + R && x < w; x++) {
      u32 dx = x > cx ? x - cx : cx - x;
      u32 dy = y > my ? y - my : my - y;
      if (dx * dx + dy * dy <= R * R) {
        u32 g = 0x99 + ((R * R - (dx * dx + dy * dy)) * 0x66) / (R * R);
        Graphics::put_pixel(x, y, (g << 16) | (g << 8) | g);
      }
    }
  }
  // mesh dots
  for (u32 y = my > R ? my - R : 0; y <= my + R && y < h; y += 3) {
    for (u32 x = cx > R ? cx - R : 0; x <= cx + R && x < w; x += 3) {
      u32 dx = x > cx ? x - cx : cx - x;
      u32 dy = y > my ? y - my : my - y;
      if (dx * dx + dy * dy <= R * R)
        Graphics::put_pixel(x, y, 0x333333);
    }
  }

  // body, stand, base
  Graphics::draw_rect(cx - 5, my + R, 10, h / 14, 0x444444);
  Graphics::draw_rect(cx - 5, my + R, 10, 3, 0x777777);
  rock_line(cx, my + R + h / 14, cx, h * 4 / 5, 0x555555);
  u32 base_y = h * 4 / 5;
  for (u32 i = 0; i < 10; i++) {
    u32 bx = cx > i ? cx - i : 0;
    for (u32 x = bx; x < cx + i && x < w; x++) {
      Graphics::put_pixel(x, base_y + i / 3, 0x333333);
    }
  }

  // cable
  u32 cy0 = my + R + h / 14 + 4;
  rock_line(cx + 5, cy0, cx + w / 8, cy0 + h / 10, 0x111111);
  rock_line(cx + w / 8, cy0 + h / 10, cx + w / 12, cy0 + h / 5, 0x111111);
  rock_line(cx + w / 12, cy0 + h / 5, cx + w / 6, h - 10, 0x111111);
}

//  Eighth notes drifting up a dark blue void.
static void draw_notes_rain() {
  u32 w = Multiboot2::framebuffer.width;
  u32 h = Multiboot2::framebuffer.height;

  for (u32 y = 0; y < h; y++) {
    for (u32 x = 0; x < w; x++) {
      u32 b = 10 + (y * 10) / h;
      Graphics::put_pixel(x, y, (2 << 16) | (3 << 8) | b);
    }
  }

  for (u32 i = 0; i < 14; i++) {
    u32 nx = 20 + ((i * 173 + 71) % (w - 40));
    u32 ny = 20 + ((i * 97 + 40) % (h * 3 / 4));
    u32 s = 1 + (i % 3);
    u32 color = (i % 4 == 0) ? 0x66ddff : 0xdddddd;

    // head
    for (u32 y = ny > 4 * s ? ny - 4 * s : 0; y <= ny + 4 * s && y < h; y++) {
      for (u32 x = nx > 4 * s ? nx - 4 * s : 0; x <= nx + 4 * s && x < w; x++) {
        u32 dx = (x > nx ? x - nx : nx - x) * 3 / 4;
        u32 dy = y > ny ? y - ny : ny - y;
        if (dx * dx + dy * dy <= (3 * s) * (3 * s))
          Graphics::put_pixel(x, y, color);
      }
    }

    // stem
    u32 stem_x = nx + 4 * s;
    for (u32 y = ny; y > ny - 14 * s && y < h; y--) {
      Graphics::put_pixel(stem_x, y, color);
    }

    // flag
    u32 flag_y = ny - 14 * s;
    for (u32 r = 0; r < 4 * s; r += 2) {
      u32 fw = (4 * s * (4 * s - r)) / (4 * s);
      if (fw < 2)
        fw = 2;
      Graphics::draw_rect(stem_x, flag_y + r, fw, 2, color);
    }
  }
}


