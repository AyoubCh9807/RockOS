
#pragma once

#include "../../data/system.hpp"
#include "../window.hpp"
#include "../window_app.hpp"

class FileExplorerApp : public IWindowApp {
private:
  static constexpr u32 BG = 0xFF171719;
  static constexpr u32 PANEL = 0xFF202024;
  static constexpr u32 PANEL_LIGHT = 0xFF303038;
  static constexpr u32 BORDER = 0xFF393940;
  static constexpr u32 TEXT = 0xFFF0F0F0;
  static constexpr u32 MUTED = 0xFF9999A3;
  static constexpr u32 GOLD = 0xFFFFC857;
  static constexpr u32 BLUE = 0xFF55B8FF;
  static constexpr u32 GREEN = 0xFF66D9A3;
  static constexpr u32 RED = 0xFFFF6666;

  static constexpr int SIDEBAR_WIDTH = 112;
  static constexpr int ROW_HEIGHT = 25;
  static constexpr int FIRST_ROW_Y = 100;

  struct FileEntry {
    const char *name;
    const char *type;
    const char *size;
    bool is_folder;
  };

  static constexpr FileEntry HOME_FILES[] = {
      {"Documents", "Folder", "--", true},
      {"Downloads", "Folder", "--", true},
      {"Music", "Folder", "--", true},
      {"Pictures", "Folder", "--", true},
      {"Projects", "Folder", "--", true},
      {"RockOS", "Folder", "--", true},
      {"README.md", "Markdown", "4 KB", false},
      {"notes.txt", "Text File", "2 KB", false},
      {"system.log", "Log File", "8 KB", false},
  };

  static constexpr FileEntry MUSIC_FILES[] = {
      {"Albums", "Folder", "--", true},
      {"Demos", "Folder", "--", true},
      {"Metallica - One.mp3", "MP3 Audio", "8 MB", false},
      {"Megadeth - Tornado.mp3", "MP3 Audio", "9 MB", false},
      {"Rock Playlist.m3u", "Playlist", "1 KB", false},
  };

  static constexpr FileEntry PROJECT_FILES[] = {
      {"boot", "Folder", "--", true},
      {"kernel", "Folder", "--", true},
      {"scripts", "Folder", "--", true},
      {"README.md", "Markdown", "6 KB", false},
      {"compile_commands.json", "JSON File", "12 KB", false},
      {"my_kernel.bin", "Binary File", "96 KB", false},
  };

  enum class Location : u8 {
    HOME,
    DOCUMENTS,
    DOWNLOADS,
    MUSIC,
    PICTURES,
    PROJECTS,
    TRASH
  };

  Location location = Location::HOME;
  int selected_index = -1;

  int get_file_count() const {
    if (location == Location::MUSIC)
      return sizeof(MUSIC_FILES) / sizeof(MUSIC_FILES[0]);

    if (location == Location::PROJECTS)
      return sizeof(PROJECT_FILES) / sizeof(PROJECT_FILES[0]);

    if (location == Location::TRASH)
      return 0;

    return sizeof(HOME_FILES) / sizeof(HOME_FILES[0]);
  }

  const FileEntry &get_file(int index) const {
    if (location == Location::MUSIC)
      return MUSIC_FILES[index];

    if (location == Location::PROJECTS)
      return PROJECT_FILES[index];

    return HOME_FILES[index];
  }

  const char *get_location_name() const {
    switch (location) {
    case Location::DOCUMENTS:
      return "Documents";
    case Location::DOWNLOADS:
      return "Downloads";
    case Location::MUSIC:
      return "Music";
    case Location::PICTURES:
      return "Pictures";
    case Location::PROJECTS:
      return "Projects";
    case Location::TRASH:
      return "Trash";
    default:
      return "Home";
    }
  }

  void draw_sidebar_item(Window &win, int y, const char *label, bool active,
                         u32 accent) {
    if (active)
      win.draw_rect(8, y - 3, SIDEBAR_WIDTH - 16, 23, PANEL_LIGHT);

    win.draw_rect(16, y + 3, 5, 5, accent);
    win.draw_string(label, 27, y, active ? TEXT : MUTED);
  }

  void draw_toolbar(Window &win, int width) {
    win.draw_rect(0, 0, width, 35, PANEL);

    win.draw_rect(10, 8, 22, 19, PANEL_LIGHT);
    win.draw_string("<", 16, 13, TEXT);

    win.draw_rect(37, 8, 22, 19, PANEL_LIGHT);
    win.draw_string(">", 43, 13, MUTED);

    win.draw_rect(66, 8, width - 78, 19, BG);
    win.draw_string("/home/rock", 73, 13, MUTED);
    win.draw_string(get_location_name(), 151, 13, TEXT);

    win.draw_rect(0, 35, width, 1, BORDER);
  }

  void draw_sidebar(Window &win, int height) {
    win.draw_rect(0, 36, SIDEBAR_WIDTH, height - 36, PANEL);

    win.draw_string("PLACES", 12, 44, MUTED);

    draw_sidebar_item(win, 61, "Home", location == Location::HOME, GOLD);
    draw_sidebar_item(win, 85, "Documents", location == Location::DOCUMENTS,
                      BLUE);
    draw_sidebar_item(win, 109, "Downloads", location == Location::DOWNLOADS,
                      GREEN);
    draw_sidebar_item(win, 133, "Music", location == Location::MUSIC, RED);
    draw_sidebar_item(win, 157, "Pictures", location == Location::PICTURES,
                      GOLD);

    win.draw_string("SYSTEM", 12, 195, MUTED);

    draw_sidebar_item(win, 213, "Projects", location == Location::PROJECTS,
                      BLUE);
    draw_sidebar_item(win, 237, "Trash", location == Location::TRASH, RED);

    win.draw_rect(8, height - 39, SIDEBAR_WIDTH - 16, 1, BORDER);
    win.draw_string("ROCK OS", 12, height - 27, GOLD);
  }

  void draw_file_icon(Window &win, int x, int y, const FileEntry &file) {
    if (file.is_folder) {
      win.draw_rect(x + 1, y + 5, 16, 11, GOLD);
      win.draw_rect(x + 2, y + 3, 7, 4, GOLD);
      win.draw_rect(x + 2, y + 7, 14, 1, 0xFFFFE29A);
      return;
    }

    u32 color = TEXT;

    if (file.type[0] == 'M')
      color = BLUE;
    else if (file.type[0] == 'P')
      color = GREEN;
    else if (file.type[0] == 'B')
      color = RED;

    win.draw_rect(x + 3, y + 2, 11, 15, color);
    win.draw_rect(x + 5, y + 6, 7, 1, BG);
    win.draw_rect(x + 5, y + 9, 7, 1, BG);
    win.draw_rect(x + 5, y + 12, 5, 1, BG);
  }

  void draw_file_list(Window &win, int width, int height) {
    int content_x = SIDEBAR_WIDTH + 1;
    int content_width = width - content_x;

    win.draw_rect(content_x, 36, content_width, height - 36, BG);

    win.draw_string(get_location_name(), content_x + 14, 44, TEXT);

    win.draw_string("Name", content_x + 14, 63, MUTED);
    win.draw_string("Type", content_x + 218, 63, MUTED);
    win.draw_string("Size", content_x + 310, 63, MUTED);

    win.draw_rect(content_x + 10, 80, content_width - 20, 1, BORDER);

    int count = get_file_count();

    for (int i = 0; i < count; i++) {
      const FileEntry &file = get_file(i);
      int y = FIRST_ROW_Y + i * ROW_HEIGHT;

      if (y + ROW_HEIGHT > height - 22)
        break;

      if (i == selected_index) {
        win.draw_rect(content_x + 7, y - 3, content_width - 14, ROW_HEIGHT,
                      0xFF353541);
      }

      draw_file_icon(win, content_x + 14, y, file);

      win.draw_string(file.name, content_x + 38, y + 3, TEXT);
      win.draw_string(file.type, content_x + 218, y + 3, MUTED);
      win.draw_string(file.size, content_x + 310, y + 3, MUTED);
    }

    win.draw_rect(content_x, height - 20, content_width, 1, BORDER);
    win.draw_string("Mock filesystem", content_x + 12, height - 14, MUTED);
  }

  void draw(Window &win) {
    int width = win.get_client_width();
    int height = win.get_client_height();

    win.clear(BG);

    draw_toolbar(win, width);
    draw_sidebar(win, height);
    draw_file_list(win, width, height);
  }

  void select_location(int y) {
    if (y >= 58 && y < 82)
      location = Location::HOME;
    else if (y >= 82 && y < 106)
      location = Location::DOCUMENTS;
    else if (y >= 106 && y < 130)
      location = Location::DOWNLOADS;
    else if (y >= 130 && y < 154)
      location = Location::MUSIC;
    else if (y >= 154 && y < 178)
      location = Location::PICTURES;
    else if (y >= 210 && y < 234)
      location = Location::PROJECTS;
    else if (y >= 234 && y < 258)
      location = Location::TRASH;

    selected_index = -1;
  }

public:
  const char *name() const override { return "File Explorer"; }

  void on_create(Window &win) override {
    location = Location::HOME;
    selected_index = -1;
    draw(win);
  }

  void on_draw(Window &win) override { draw(win); }

  void on_mouse_event(Window &win, const MouseEvent &ev) override {
    if (ev.button_type != MouseButton::LEFT_BUTTON ||
        ev.event_type != MouseEventType::PRESS)
      return;

    int x = Mouse::get_x();
    int y = Mouse::get_y();

    if (x < SIDEBAR_WIDTH && y >= 58 && y < 258) {
      select_location(y);
      draw(win);
      return;
    }

    if (x >= SIDEBAR_WIDTH + 1 && y >= FIRST_ROW_Y) {
      int index = (y - FIRST_ROW_Y) / ROW_HEIGHT;

      if (index >= 0 && index < get_file_count())
        selected_index = index;
      else
        selected_index = -1;

      draw(win);
    }
  }
};
