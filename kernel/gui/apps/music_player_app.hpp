
#pragma once

#include "../../data/system.hpp"
#include "../window.hpp"
#include "../window_app.hpp"

class MusicPlayerApp : public IWindowApp {
private:
  static constexpr u32 BG = 0x111116;
  static constexpr u32 PANEL = 0x1A1A22;
  static constexpr u32 PANEL_LIGHT = 0x282832;
  static constexpr u32 BORDER = 0x383844;
  static constexpr u32 TEXT = 0xF0F0F5;
  static constexpr u32 MUTED = 0x9292A0;
  static constexpr u32 GOLD = 0xF5B82E;
  static constexpr u32 RED = 0xE65A60;
  static constexpr u32 GREEN = 0x61C995;

  static constexpr int HEADER_HEIGHT = 40;
  static constexpr int SIDEBAR_WIDTH = 150;
  static constexpr int FOOTER_HEIGHT = 76;

  struct Track {
    const char *title;
    const char *artist;
    const char *album;
    const char *duration;
    u32 color;
  };

  static constexpr Track TRACKS[] = {
      {"Chop Suey!", "System Of A Down", "Toxicity", "3:30", 0xD95B45},
      {"Master Of Puppets", "Metallica", "Master Of Puppets", "8:35", 0xC5A66A},
      {"Duality", "Slipknot", "Vol. 3", "3:33", 0x777777},
      {"The Trooper", "Iron Maiden", "Piece Of Mind", "4:12", 0x61A8C9},
      {"Psychosocial", "Slipknot", "All Hope Is Gone", "4:43", 0xA84D54},
      {"Holy Wars", "Megadeth", "Rust In Peace", "6:36", 0x65A17D},
      {"Paranoid", "Black Sabbath", "Paranoid", "2:48", 0xB69AFF},
      {"One", "Metallica", "...And Justice For All", "7:27", 0x7B8ED6},
  };

  static constexpr int TRACK_COUNT = sizeof(TRACKS) / sizeof(TRACKS[0]);

  int current_track = 0;
  int selected_track = 0;
  int progress = 0;
  int volume = 70;

  bool playing = false;
  bool shuffled = false;
  bool repeat = false;

  int list_y = 0;
  int list_row_height = 43;

  void fill(Window &win, int x, int y, int width, int height, u32 color) {
    win.draw_rect(x, y, width, height, color);
  }

  void outline(Window &win, int x, int y, int width, int height, u32 color) {
    if (width <= 0 || height <= 0)
      return;

    fill(win, x, y, width, 1, color);
    fill(win, x, y + height - 1, width, 1, color);
    fill(win, x, y, 1, height, color);
    fill(win, x + width - 1, y, 1, height, color);
  }

  void centered(Window &win, const char *text, int center_x, int y, u32 color) {
    if (!text)
      return;

    int length = 0;
    while (text[length])
      length++;

    win.draw_string(text, center_x - length * 4, y, color);
  }

  bool inside(int px, int py, int x, int y, int width, int height) {
    return px >= x && px < x + width && py >= y && py < y + height;
  }

  void draw_button(Window &win, int x, int y, int width, int height,
                   const char *text, u32 color = TEXT) {
    fill(win, x, y, width, height, PANEL_LIGHT);
    centered(win, text, x + width / 2, y + (height - 8) / 2, color);
  }

  void draw_header(Window &win, int width) {
    fill(win, 0, 0, width, HEADER_HEIGHT, PANEL);
    fill(win, 0, HEADER_HEIGHT - 1, width, 1, BORDER);

    fill(win, 12, 9, 22, 22, GOLD);
    fill(win, 16, 13, 14, 14, BG);
    win.draw_string("R", 19, 16, GOLD);

    win.draw_string("ROCK", 42, 8, TEXT);
    win.draw_string("PLAYER", 42, 21, GOLD);

    win.draw_string("YOUR MUSIC.", SIDEBAR_WIDTH + 18, 16, MUTED);
  }

  void draw_sidebar(Window &win, int height) {
    fill(win, 0, HEADER_HEIGHT, SIDEBAR_WIDTH,
         height - HEADER_HEIGHT - FOOTER_HEIGHT, PANEL);

    const int x = 14;
    int y = HEADER_HEIGHT + 20;

    win.draw_string("LIBRARY", x, y, MUTED);
    y += 26;

    fill(win, 8, y - 5, SIDEBAR_WIDTH - 16, 27, PANEL_LIGHT);
    fill(win, 8, y - 5, 3, 27, GOLD);
    win.draw_string("M", x + 7, y + 3, GOLD);
    win.draw_string("All Tracks", x + 25, y + 3, TEXT);

    y += 39;
    win.draw_string("P", x + 7, y + 3, MUTED);
    win.draw_string("Playlists", x + 25, y + 3, MUTED);

    y += 39;
    win.draw_string("A", x + 7, y + 3, MUTED);
    win.draw_string("Albums", x + 25, y + 3, MUTED);

    y += 39;
    win.draw_string("F", x + 7, y + 3, MUTED);
    win.draw_string("Favorites", x + 25, y + 3, MUTED);

    y += 32;
    fill(win, 12, y, SIDEBAR_WIDTH - 24, 1, BORDER);

    y += 20;
    win.draw_string("PLAYLISTS", x, y, MUTED);

    y += 27;
    fill(win, x, y, 8, 8, RED);
    win.draw_string("Heavy Rotation", x + 15, y, TEXT);

    y += 25;
    fill(win, x, y, 8, 8, GOLD);
    win.draw_string("Classics", x + 15, y, MUTED);

    y += 25;
    fill(win, x, y, 8, 8, GREEN);
    win.draw_string("Late Night", x + 15, y, MUTED);
  }

  void draw_album_art(Window &win, int x, int y, int size) {
    const Track &track = TRACKS[current_track];

    fill(win, x, y, size, size, track.color);

    // Abstract metal-inspired album artwork.
    fill(win, x + size / 2 - 2, y + 8, 4, size - 16, BG);
    fill(win, x + 8, y + size / 2 - 2, size - 16, 4, BG);

    fill(win, x + 12, y + 12, size - 24, size - 24, track.color);
    outline(win, x + 12, y + 12, size - 24, size - 24, BG);

    centered(win, "R", x + size / 2, y + size / 2 - 4, TEXT);
  }

  void draw_now_playing(Window &win, int x, int y, int width) {
    const Track &track = TRACKS[current_track];

    win.draw_string("NOW PLAYING", x, y, GOLD);

    draw_album_art(win, x, y + 24, 104);

    win.draw_string(track.title, x + 119, y + 30, TEXT);
    win.draw_string(track.artist, x + 119, y + 49, MUTED);

    win.draw_string("ALBUM", x + 119, y + 79, GOLD);
    win.draw_string(track.album, x + 119, y + 93, TEXT);

    const int info_width = width - 143;

    if (info_width > 70) {
      fill(win, x + 119, y + 120, info_width, 1, BORDER);
      fill(win, x + 119, y + 120, info_width * progress / 100, 2, GOLD);
    }
  }

  void draw_track_list(Window &win, int width, int height) {
    const int x = SIDEBAR_WIDTH + 18;
    const int right = width - 12;

    list_y = HEADER_HEIGHT + 174;

    win.draw_string("UP NEXT", x, list_y - 21, GOLD);

    fill(win, x, list_y, right - x, 25, PANEL);
    win.draw_string("#", x + 5, list_y + 8, MUTED);
    win.draw_string("TITLE", x + 32, list_y + 8, MUTED);
    win.draw_string("ALBUM", x + 200, list_y + 8, MUTED);
    win.draw_string("TIME", right - 40, list_y + 8, MUTED);

    list_y += 25;

    const int available_bottom = height - FOOTER_HEIGHT - 8;
    const int visible_count = (available_bottom - list_y) / list_row_height;

    for (int i = 0; i < TRACK_COUNT && i < visible_count; i++) {
      const Track &track = TRACKS[i];
      const int y = list_y + i * list_row_height;
      const bool active = i == current_track;
      const bool selected = i == selected_track;

      if (selected)
        fill(win, x, y, right - x, list_row_height, PANEL_LIGHT);

      if (active)
        fill(win, x, y, 3, list_row_height, GOLD);

      char number[2] = {(char)('1' + i), '\0'};

      win.draw_string(active && playing ? ">" : number, x + 5, y + 9,
                      active ? GOLD : MUTED);

      win.draw_string(track.title, x + 32, y + 6, active ? GOLD : TEXT);
      win.draw_string(track.artist, x + 32, y + 22, MUTED);

      win.draw_string(track.album, x + 200, y + 14, MUTED);
      win.draw_string(track.duration, right - 40, y + 14, MUTED);

      fill(win, x, y + list_row_height - 1, right - x, 1, BORDER);
    }
  }

  void draw_footer(Window &win, int width, int height) {
    const int y = height - FOOTER_HEIGHT;
    const Track &track = TRACKS[current_track];

    fill(win, 0, y, width, FOOTER_HEIGHT, PANEL);
    fill(win, 0, y, width, 1, BORDER);

    fill(win, 12, y + 14, 46, 46, track.color);
    centered(win, "R", 35, y + 33, TEXT);

    win.draw_string(track.title, 68, y + 19, TEXT);
    win.draw_string(track.artist, 68, y + 38, MUTED);

    const int center_x = width / 2;

    draw_button(win, center_x - 87, y + 12, 26, 25, shuffled ? "S" : "s",
                shuffled ? GOLD : MUTED);

    draw_button(win, center_x - 53, y + 12, 27, 25, "|<");

    fill(win, center_x - 17, y + 9, 34, 32, GOLD);
    centered(win, playing ? "||" : ">", center_x, y + 21, BG);

    draw_button(win, center_x + 26, y + 12, 27, 25, ">|");

    draw_button(win, center_x + 60, y + 12, 26, 25, repeat ? "R" : "r",
                repeat ? GOLD : MUTED);

    const int progress_width = width > 520 ? 220 : 100;
    const int progress_x = center_x - progress_width / 2;

    fill(win, progress_x, y + 48, progress_width, 3, BORDER);
    fill(win, progress_x, y + 48, progress_width * progress / 100, 3, GOLD);

    win.draw_string("0:00", progress_x - 36, y + 45, MUTED);

    const int volume_x = width - 110;

    win.draw_string("VOL", volume_x, y + 15, MUTED);
    fill(win, volume_x, y + 34, 78, 3, BORDER);
    fill(win, volume_x, y + 34, 78 * volume / 100, 3, GOLD);

    fill(win, volume_x + 78 * volume / 100 - 2, y + 30, 6, 11, TEXT);
  }

  void next_track() {
    current_track = (current_track + 1) % TRACK_COUNT;
    selected_track = current_track;
    progress = 0;
    playing = true;
  }

  void previous_track() {
    if (progress > 5) {
      progress = 0;
      return;
    }

    current_track = (current_track + TRACK_COUNT - 1) % TRACK_COUNT;

    selected_track = current_track;
    progress = 0;
    playing = true;
  }

  void select_track(int index) {
    if (index < 0 || index >= TRACK_COUNT)
      return;

    current_track = index;
    selected_track = index;
    progress = 0;
    playing = true;
  }

  void draw(Window &win) {
    const int width = win.get_width();
    const int height = win.get_height();

    if (width < 300 || height < 240)
      return;

    win.clear(BG);

    draw_header(win, width);
    draw_sidebar(win, height);
    draw_now_playing(win, SIDEBAR_WIDTH + 18, HEADER_HEIGHT + 17,
                     width - SIDEBAR_WIDTH - 30);
    draw_track_list(win, width, height);
    draw_footer(win, width, height);
  }

public:
  const char *name() const override { return "Music Player"; }

  void on_create(Window &win) override { draw(win); }

  void on_draw(Window &win) override { draw(win); }

  void on_key(Window &win, const KeyEvent &ev) override {
    if (ev.scancode == 32) {
      playing = !playing;
    } else if (ev.scancode == (int)'n' || ev.scancode == (int)'N') {
      next_track();
    } else if (ev.scancode == (int)'p' || ev.scancode == (int)'P') {
      previous_track();
    } else if (ev.scancode == (int)'s' || ev.scancode == (int)'S') {
      shuffled = !shuffled;
    } else if (ev.scancode == (int)'r' || ev.scancode == (int)'R') {
      repeat = !repeat;
    }

    draw(win);
  }

  void on_mouse_event(Window &win, const MouseEvent &ev) override {
    if (ev.button_type != MouseButton::LEFT_BUTTON ||
        ev.event_type != MouseEventType::PRESS)
      return;

    const int mx = Mouse::get_x();
    const int my = Mouse::get_y();

    const int width = win.get_width();
    const int height = win.get_height();
    const int footer_y = height - FOOTER_HEIGHT;
    const int center_x = width / 2;

    // Track selection.
    const int x = SIDEBAR_WIDTH + 18;
    const int right = width - 12;
    const int first_row = list_y + 25;
    const int available_bottom = height - FOOTER_HEIGHT - 8;
    const int visible_count = (available_bottom - first_row) / list_row_height;

    for (int i = 0; i < TRACK_COUNT && i < visible_count; i++) {
      const int row_y = first_row + i * list_row_height;

      if (inside(mx, my, x, row_y, right - x, list_row_height)) {
        select_track(i);
        draw(win);
        return;
      }
    }

    if (my >= footer_y + 8 && my < footer_y + 44) {
      if (inside(mx, my, center_x - 87, footer_y + 12, 26, 25)) {
        shuffled = !shuffled;
      } else if (inside(mx, my, center_x - 53, footer_y + 12, 27, 25)) {
        previous_track();
      } else if (inside(mx, my, center_x - 17, footer_y + 9, 34, 32)) {
        playing = !playing;
      } else if (inside(mx, my, center_x + 26, footer_y + 12, 27, 25)) {
        next_track();
      } else if (inside(mx, my, center_x + 60, footer_y + 12, 26, 25)) {
        repeat = !repeat;
      }
    }

    if (inside(mx, my, width - 110, footer_y + 25, 80, 22)) {
      volume = (mx - (width - 110)) * 100 / 78;

      if (volume < 0)
        volume = 0;

      if (volume > 100)
        volume = 100;
    }

    draw(win);
  }
};
