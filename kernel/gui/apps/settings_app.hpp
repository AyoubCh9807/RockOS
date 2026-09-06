#pragma once

#include "../../data/system.hpp"
#include "../../tyrant_generator/generator.hpp"
#include "../window.hpp"
#include "../window_app.hpp"

#include "../../shared/key_event.hpp"

class SettingsApp : public IWindowApp {
private:
  u32 fill_color = Colors::RED;

  enum Tab { TAB_WALLPAPER, TAB_APPEARANCE, TAB_SYSTEM, TAB_ABOUT, TAB_COUNT };

  int selected_tab = TAB_WALLPAPER;
  static constexpr int TAB_HEIGHT = 32;
  static constexpr int TAB_WIDTH = 100;

  static constexpr u32 TAB_BAR_COLOR = 0x202020;
  static constexpr u32 TAB_COLOR = 0x282828;
  static constexpr u32 CONTENT_COLOR = 0x181818;
  static constexpr u32 SELECTED_COLOR = 0x353535;
  static constexpr u32 TEXT_COLOR = 0xFFFFFF;
  static constexpr u32 MUTED_TEXT = 0x999999;
  static constexpr u32 ACCENT_COLOR = Colors::GOLD;

  void select_next_tab() {
    selected_tab = static_cast<Tab>((selected_tab + 1) % TAB_COUNT);
  }

  void select_previous_tab() {
    selected_tab = static_cast<Tab>((selected_tab + TAB_COUNT - 1) % TAB_COUNT);
  }

  void highlight_tab(Window &win, int tab) {
    if (tab != selected_tab)
      return;

    int x = 2 + tab * TAB_WIDTH;

    // Make the selected tab stand out from the rest.
    win.draw_rect(x, 2, TAB_WIDTH, TAB_HEIGHT, SELECTED_COLOR);

    // Gold accent under the selected tab.
    win.draw_rect(x + 6, TAB_HEIGHT - 3, TAB_WIDTH - 12, 3, ACCENT_COLOR);
  }

  void draw_tabs(Window &win) {
    win.draw_rect(2, 2, win.width - 4, TAB_HEIGHT, TAB_BAR_COLOR);

    for (int i = 0; i < TAB_COUNT; i++) {
      int x = 2 + i * TAB_WIDTH;

      win.draw_rect(x, 2, TAB_WIDTH, TAB_HEIGHT, TAB_COLOR);
    }

    highlight_tab(win, selected_tab);

    win.draw_string("Wallpaper", 15, 12, TEXT_COLOR);
    win.draw_string("Appearance", 115, 12, TEXT_COLOR);
    win.draw_string("System", 215, 12, TEXT_COLOR);
    win.draw_string("About", 315, 12, TEXT_COLOR);
  }

  void draw_content_background(Window &win) {
    // Keep the actual settings content below the tabs.
    win.draw_rect(10, 50, win.width - 20, win.height - 60, CONTENT_COLOR);
  }

  void draw_section_title(Window &win, const char *title, int y) {
    win.draw_string(title, 25, y, TEXT_COLOR);

    win.draw_rect(25, y + 18, 40, 2, ACCENT_COLOR);
  }

  void draw_wallpaper_options(Window &win) {
    draw_content_background(win);

    draw_section_title(win, "Wallpaper", 65);

    win.draw_string("Choose your desktop background", 25, 95, MUTED_TEXT);

    // This is just a placeholder for the real wallpaper preview.
    win.draw_rect(25, 125, 220, 110, 0x252525);

    win.draw_rect(30, 130, 210, 100, 0x351515);

    win.draw_string("Current Wallpaper", 40, 175, TEXT_COLOR);

    // These will eventually cycle through the wallpapers.
    win.draw_rect(25, 250, 90, 30, TAB_COLOR);

    win.draw_rect(125, 250, 90, 30, TAB_COLOR);

    win.draw_string("Previous", 37, 260, TEXT_COLOR);

    win.draw_string("Next", 153, 260, TEXT_COLOR);
  }

  void draw_appearance_options(Window &win) {
    draw_content_background(win);

    draw_section_title(win, "Appearance", 65);

    win.draw_string("Theme", 25, 105, TEXT_COLOR);

    win.draw_string("Choose how Rock OS looks", 25, 130, MUTED_TEXT);

    win.draw_rect(25, 155, 160, 32, TAB_COLOR);

    win.draw_string("Dark Theme", 38, 166, TEXT_COLOR);

    win.draw_string("Accent Color", 25, 215, TEXT_COLOR);

    // Show the current accent color.
    win.draw_rect(25, 240, 40, 25, ACCENT_COLOR);

    win.draw_string("Window Borders", 100, 250, TEXT_COLOR);
  }

  void draw_system_options(Window &win) {
    draw_content_background(win);

    draw_section_title(win, "System", 65);

    win.draw_string("Display", 25, 105, TEXT_COLOR);

    win.draw_string("Resolution", 25, 135, MUTED_TEXT);

    win.draw_string("Framebuffer", 130, 135, TEXT_COLOR);

    win.draw_string("Performance", 25, 180, TEXT_COLOR);

    win.draw_string("FPS", 25, 210, MUTED_TEXT);

    win.draw_string("50", 130, 210, TEXT_COLOR);

    win.draw_string("Keyboard", 25, 255, TEXT_COLOR);

    win.draw_string("AZERTY", 130, 255, TEXT_COLOR);
  }

  void draw_about_options(Window &win) {
    draw_content_background(win);

    draw_section_title(win, "About Rock OS", 65);

    win.draw_string("Rock OS", 25, 110, TEXT_COLOR);

    win.draw_string("A tiny operating system built from scratch.", 25, 140,
                    MUTED_TEXT);

    win.draw_string("Version", 25, 190, MUTED_TEXT);

    win.draw_string("0.1.0", 130, 190, TEXT_COLOR);

    win.draw_string("Architecture", 25, 220, MUTED_TEXT);

    win.draw_string("x86_64", 130, 220, TEXT_COLOR);

    win.draw_string("Made with C++", 25, 265, ACCENT_COLOR);
  }

  void draw_options(Window &win) {
    switch (selected_tab) {
    case TAB_WALLPAPER:
      draw_wallpaper_options(win);
      break;

    case TAB_APPEARANCE:
      draw_appearance_options(win);
      break;

    case TAB_SYSTEM:
      draw_system_options(win);
      break;

    case TAB_ABOUT:
      draw_about_options(win);
      break;

    default:
      break;
    }
  }

public:
  const char *name() const override { return "Settings"; }

  void on_create(Window &win) override { draw(win); }

  void on_draw(Window &win) override { draw(win); }

  void draw(Window &win) {
    win.clear(fill_color);

    // Draw the border one pixel at a time so it stays inside
    // the window's own buffer.
    for (int x = 0; x < win.width; x++) {
      win.set_pixel(x, 0, 0xFFFFFF);
      win.set_pixel(x, 1, 0xFFFFFF);

      win.set_pixel(x, win.height - 1, 0xFFFFFF);

      win.set_pixel(x, win.height - 2, 0xFFFFFF);
    }

    for (int y = 0; y < win.height; y++) {
      win.set_pixel(0, y, 0xFFFFFF);
      win.set_pixel(1, y, 0xFFFFFF);

      win.set_pixel(win.width - 1, y, 0xFFFFFF);

      win.set_pixel(win.width - 2, y, 0xFFFFFF);
    }

    draw_tabs(win);
    draw_options(win);
  }

  void on_key(Window &win, const KeyEvent &ev) override {

    if (ev.keytype == KeyType::ArrowRight) {
      select_next_tab();
      draw(win);
      return;
    }

    if (ev.keytype == KeyType::ArrowLeft) {
      select_previous_tab();
      draw(win);
      return;
    }

    return;
  }
};
