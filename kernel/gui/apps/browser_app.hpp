#pragma once

#include "../../data/system.hpp"
#include "../window.hpp"
#include "../window_app.hpp"

class BrowserApp : public IWindowApp {
private:
  static constexpr u32 BG = 0x111116;
  static constexpr u32 PANEL = 0x1A1A22;
  static constexpr u32 PANEL_LIGHT = 0x262630;
  static constexpr u32 BORDER = 0x383844;
  static constexpr u32 TEXT = 0xF0F0F5;
  static constexpr u32 MUTED = 0x9292A0;
  static constexpr u32 GOLD = 0xF5B82E;
  static constexpr u32 BLUE = 0x73A7FF;
  static constexpr u32 RED = 0xE65A60;
  static constexpr u32 GREEN = 0x61C995;

  static constexpr int TITLE_BAR_HEIGHT = 24;
  static constexpr int TAB_BAR_HEIGHT = 32;
  static constexpr int TOOLBAR_HEIGHT = 42;
  static constexpr int BOOKMARKS_HEIGHT = 27;
  static constexpr int STATUS_HEIGHT = 19;

  static constexpr int ADDRESS_CAPACITY = 256;

  char address[ADDRESS_CAPACITY] = "rock://newtab";
  char status[80] = "Ready";
  char search_text[ADDRESS_CAPACITY] = {};

  bool address_focused = false;
  bool search_focused = false;

  int address_length = 13;
  int search_length = 0;

  int active_tab = 0;

  int address_x = 143;
  int address_y = 0;
  int address_width = 0;

  int search_x = 0;
  int search_y = 0;
  int search_width = 0;

  int content_top = 0;

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

  void centered_text(Window &win, const char *text, int center_x, int y,
                     u32 color) {
    if (!text)
      return;

    int length = 0;

    while (text[length])
      length++;

    win.draw_string(text, center_x - length * 4, y, color);
  }

  void draw_button(Window &win, int x, int y, int width, int height,
                   const char *text, u32 color = TEXT) {
    fill(win, x, y, width, height, PANEL_LIGHT);
    centered_text(win, text, x + width / 2, y + (height - 8) / 2, color);
  }

  bool inside(int px, int py, int x, int y, int width, int height) {
    return px >= x && px < x + width && py >= y && py < y + height;
  }

  void set_status(const char *message) {
    int i = 0;

    while (message[i] && i < (int)sizeof(status) - 1) {
      status[i] = message[i];
      i++;
    }

    status[i] = '\0';
  }

  void draw_tab(Window &win, int width) {
    fill(win, 0, TITLE_BAR_HEIGHT, width, TAB_BAR_HEIGHT, PANEL);

    fill(win, 8, TITLE_BAR_HEIGHT, 150, 31, BG);
    fill(win, 8, TITLE_BAR_HEIGHT + 29, 150, 2, GOLD);

    fill(win, 19, TITLE_BAR_HEIGHT + 10, 10, 10, GOLD);
    fill(win, 22, TITLE_BAR_HEIGHT + 13, 4, 4, BG);

    win.draw_string("New Tab", 36, TITLE_BAR_HEIGHT + 11, TEXT);
    win.draw_string("x", 143, TITLE_BAR_HEIGHT + 11, MUTED);

    draw_button(win, 169, TITLE_BAR_HEIGHT + 5, 23, 21, "+", MUTED);
  }

  void draw_toolbar(Window &win, int width) {
    const int y = TITLE_BAR_HEIGHT + TAB_BAR_HEIGHT;

    fill(win, 0, y, width, TOOLBAR_HEIGHT, BG);

    draw_button(win, 8, y + 8, 27, 26, "<");
    draw_button(win, 40, y + 8, 27, 26, ">", MUTED);
    draw_button(win, 72, y + 8, 27, 26, "R");
    draw_button(win, 104, y + 8, 27, 26, "H");

    address_x = 143;
    address_y = y + 6;
    address_width = width - address_x - 48;

    if (address_width < 0)
      address_width = 0;

    if (address_width > 0) {
      fill(win, address_x, address_y, address_width, 30, PANEL);
      outline(win, address_x, address_y, address_width, 30,
              address_focused ? GOLD : BORDER);

      // Lock icon.
      outline(win, address_x + 10, address_y + 12, 10, 9, GREEN);
      outline(win, address_x + 12, address_y + 8, 6, 6, GREEN);

      const char *shown_address = address_focused ? address : address;

      win.draw_string(shown_address, address_x + 30, address_y + 11, TEXT);

      if (address_focused) {
        int cursor_x = address_x + 30 + address_length * 8;

        if (cursor_x < address_x + address_width - 4)
          fill(win, cursor_x, address_y + 9, 1, 12, GOLD);
      }
    }

    draw_button(win, width - 40, y + 9, 28, 24, "...", TEXT);
  }

  void draw_bookmarks(Window &win, int width) {
    const int y = TITLE_BAR_HEIGHT + TAB_BAR_HEIGHT + TOOLBAR_HEIGHT;

    fill(win, 0, y, width, BOOKMARKS_HEIGHT, PANEL);
    fill(win, 0, y + BOOKMARKS_HEIGHT - 1, width, 1, BORDER);

    win.draw_string("*", 12, y + 9, GOLD);
    win.draw_string("Bookmarks", 25, y + 9, TEXT);
    win.draw_string("|", 110, y + 9, BORDER);
    win.draw_string("Getting Started", 126, y + 9, MUTED);
    win.draw_string("Rock OS", 246, y + 9, MUTED);
  }

  void draw_search(Window &win, int center_x, int y) {
    search_width = win.get_width() - 80;

    if (search_width > 560)
      search_width = 560;

    if (search_width < 180)
      search_width = 180;

    search_x = center_x - search_width / 2;
    search_y = y;

    fill(win, search_x, search_y, search_width, 40, PANEL);
    outline(win, search_x, search_y, search_width, 40,
            search_focused ? GOLD : BORDER);

    outline(win, search_x + 14, search_y + 12, 10, 10, MUTED);
    fill(win, search_x + 23, search_y + 21, 5, 2, MUTED);

    if (search_length > 0) {
      win.draw_string(search_text, search_x + 38, search_y + 16, TEXT);
    } else {
      win.draw_string("Search or enter an address...", search_x + 38,
                      search_y + 16, MUTED);
    }

    if (search_focused) {
      int cursor_x = search_x + 38 + search_length * 8;

      if (cursor_x < search_x + search_width - 8)
        fill(win, cursor_x, search_y + 13, 1, 12, GOLD);
    }
  }

  void draw_shortcut(Window &win, int x, int y, int width, const char *letter,
                     const char *name, u32 accent) {
    fill(win, x, y, width, 78, PANEL);
    outline(win, x, y, width, 78, BORDER);

    fill(win, x + 12, y + 12, 30, 30, accent);
    centered_text(win, letter, x + 27, y + 23, BG);

    win.draw_string(name, x + 12, y + 53, TEXT);
  }

  void draw_home(Window &win, int width, int height) {
    const int center_x = width / 2;

    content_top =
        TITLE_BAR_HEIGHT + TAB_BAR_HEIGHT + TOOLBAR_HEIGHT + BOOKMARKS_HEIGHT;

    fill(win, 0, content_top, width, height - content_top, BG);

    // Brand mark.
    fill(win, center_x - 28, content_top + 36, 56, 56, GOLD);
    fill(win, center_x - 20, content_top + 44, 40, 40, BG);

    win.draw_string("R", center_x - 12, content_top + 60, GOLD);

    centered_text(win, "ROCK BROWSER", center_x, content_top + 111, TEXT);

    centered_text(win, "THE WEB, AT YOUR FINGERTIPS", center_x,
                  content_top + 131, MUTED);

    draw_search(win, center_x, content_top + 160);

    centered_text(win, "QUICK ACCESS", center_x, content_top + 216, MUTED);

    const int card_width = 112;
    const int gap = 12;
    const int total_width = card_width * 4 + gap * 3;
    const int first_x = center_x - total_width / 2;
    const int card_y = content_top + 239;

    draw_shortcut(win, first_x, card_y, card_width, "G", "Google", BLUE);

    draw_shortcut(win, first_x + card_width + gap, card_y, card_width, "Y",
                  "YouTube", RED);

    draw_shortcut(win, first_x + (card_width + gap) * 2, card_y, card_width,
                  "G", "GitHub", TEXT);

    draw_shortcut(win, first_x + (card_width + gap) * 3, card_y, card_width,
                  "R", "Rock OS", GOLD);

    if (height - content_top > 360) {
      centered_text(win, "BUILT FOR THE SPIRIT OF EXPLORATION", center_x,
                    height - STATUS_HEIGHT - 29, MUTED);
    }
  }

  void draw_status(Window &win, int width, int height) {
    fill(win, 0, height - STATUS_HEIGHT, width, STATUS_HEIGHT, PANEL);
    fill(win, 0, height - STATUS_HEIGHT, width, 1, BORDER);

    fill(win, 10, height - 12, 5, 5, GREEN);
    win.draw_string(status, 22, height - 14, MUTED);

    win.draw_string("ROCK ENGINE", width - 112, height - 14, GOLD);
  }

  void draw(Window &win) {
    const int width = win.get_width();
    const int height = win.get_height();

    if (width < 240 || height < 180)
      return;

    win.clear(BG);

    draw_tab(win, width);
    draw_toolbar(win, width);
    draw_bookmarks(win, width);
    draw_home(win, width, height);
    draw_status(win, width, height);
  }

  void submit_search() {
    const char *text = search_length > 0 ? search_text : address;

    if (text[0] == '\0') {
      set_status("Enter an address or search term");
      return;
    }

    // Navigation will be implemented later.
    set_status("Navigation is not implemented yet");

    address_focused = false;
    search_focused = false;
  }

  void append_character(char c) {
    if (address_focused) {
      if (address_length >= ADDRESS_CAPACITY - 1)
        return;

      address[address_length++] = c;
      address[address_length] = '\0';
    } else if (search_focused) {
      if (search_length >= ADDRESS_CAPACITY - 1)
        return;

      search_text[search_length++] = c;
      search_text[search_length] = '\0';
    }
  }

  void remove_character() {
    if (address_focused && address_length > 0) {
      address[--address_length] = '\0';
    } else if (search_focused && search_length > 0) {
      search_text[--search_length] = '\0';
    }
  }

public:
  const char *name() const override { return "Browser"; }

  void on_create(Window &win) override { draw(win); }

  void on_draw(Window &win) override { draw(win); }

  void on_key(Window &win, const KeyEvent &ev) override {
    if (ev.scancode == 27) {
      address_focused = false;
      search_focused = false;
      set_status("Ready");
    } else if (ev.scancode == 8) {
      remove_character();
    } else if (ev.scancode == 13) {
      submit_search();
    } else if (ev.scancode == (int)'r' || ev.scancode == (int)'R') {
      if (!address_focused && !search_focused)
        set_status("Refresh is not implemented yet");
    } else if (ev.scancode == (int)'l' || ev.scancode == (int)'L') {
      address_focused = true;
      search_focused = false;
      set_status("Enter an address");
    } else if (ev.scancode >= 32 && ev.scancode <= 126) {
      append_character((char)ev.scancode);
    }

    draw(win);
  }

  void on_mouse_event(Window &win, const MouseEvent &ev) override {
    if (ev.button_type != MouseButton::LEFT_BUTTON ||
        ev.event_type != MouseEventType::PRESS)
      return;

    const int mx = Mouse::get_x();
    const int my = Mouse::get_y();

    const int toolbar_y = TITLE_BAR_HEIGHT + TAB_BAR_HEIGHT;
    const int bookmarks_y = toolbar_y + TOOLBAR_HEIGHT;
    const int width = win.get_width();

    if (inside(mx, my, address_x, address_y, address_width, 30)) {
      address_focused = true;
      search_focused = false;
      set_status("Enter an address");
    } else if (inside(mx, my, search_x, search_y, search_width, 40)) {
      search_focused = true;
      address_focused = false;
      set_status("Search the web");
    } else if (inside(mx, my, 8, toolbar_y + 8, 27, 26)) {
      set_status("Back is not implemented yet");
    } else if (inside(mx, my, 40, toolbar_y + 8, 27, 26)) {
      set_status("Forward is not implemented yet");
    } else if (inside(mx, my, 72, toolbar_y + 8, 27, 26)) {
      set_status("Refresh is not implemented yet");
    } else if (inside(mx, my, 104, toolbar_y + 8, 27, 26)) {
      address_length = 13;

      const char *home = "rock://newtab";
      for (int i = 0; i < address_length; i++)
        address[i] = home[i];

      address[address_length] = '\0';
      search_length = 0;
      search_text[0] = '\0';
      address_focused = false;
      search_focused = false;
      set_status("Home");
    } else if (inside(mx, my, 169, TITLE_BAR_HEIGHT + 5, 23, 21)) {
      set_status("New tabs are not implemented yet");
    } else if (inside(mx, my, width - 40, toolbar_y + 9, 28, 24)) {
      set_status("Browser menu is not implemented yet");
    } else if (inside(mx, my, 0, bookmarks_y, width, BOOKMARKS_HEIGHT)) {
      set_status("Bookmark selected");
    } else {
      address_focused = false;
      search_focused = false;
    }

    draw(win);
  }
};
