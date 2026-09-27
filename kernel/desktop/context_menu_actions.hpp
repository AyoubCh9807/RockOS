#pragma once
#include "desktop_actions.hpp"

using Action = void (*)(void *context);

enum class ContextMenuActions : u8 {
  NONE,

  OPEN_LAUNCHER,
  NEXT_WALLPAPER,

  OPEN_SETTINGS,
  OPEN_ABOUT,

  MINIMIZE_WINDOW,
  CLOSE_WINDOW,

  REFRESH_DESKTOP,

  TOGGLE_CURSOR,
  TOGGLE_WALLPAPER,

  OPEN_TERMINAL,
  OPEN_FILE_MANAGER,
  OPEN_BROWSER,
  OPEN_CALCULATOR,
  OPEN_ROCK_AI
};

inline void open_launcher(void *context) {
  if (!context)
    return;

  DesktopActions *actions = static_cast<DesktopActions *>(context);
  actions->open_launcher();
}

inline void next_wallpaper(void *context) {
  if (!context)
    return;

  DesktopActions *actions = static_cast<DesktopActions *>(context);
  actions->next_wallpaper();
}

inline void open_settings(void *context) {
  if (!context)
    return;

  DesktopActions *actions = static_cast<DesktopActions *>(context);
  actions->get_app_launcher().launch_app("Settings");
}

inline void open_about(void *context) {
  if (!context)
    return;

  DesktopActions *actions = static_cast<DesktopActions *>(context);
  actions->get_app_launcher().launch_app("About");
}

inline void minimize_window(void *context) {
  if (!context)
    return;

  DesktopActions *actions = static_cast<DesktopActions *>(context);
  actions->minimize_focused_window();
}

inline void close_window(void *context) {
  if (!context)
    return;

  DesktopActions *actions = static_cast<DesktopActions *>(context);
  actions->close_focused_window();
}

inline void refresh_desktop(void *context) {
  if (!context)
    return;

  DesktopActions *actions = static_cast<DesktopActions *>(context);

  // actions->refresh_desktop();
}

inline void toggle_cursor(void *context) {
  if (!context)
    return;

  DesktopActions *actions = static_cast<DesktopActions *>(context);
  // actions->toggle_cursor();
}

inline void toggle_wallpaper(void *context) {
  if (!context)
    return;

  DesktopActions *actions = static_cast<DesktopActions *>(context);
  // actions->toggle_wallpaper();
}

inline void open_terminal(void *context) {
  if (!context)
    return;

  DesktopActions *actions = static_cast<DesktopActions *>(context);
  actions->open_app("Terminal");
}

inline void open_file_manager(void *context) {
  if (!context)
    return;

  DesktopActions *actions = static_cast<DesktopActions *>(context);
  actions->open_app("File Manager");
}

inline void open_browser(void *context) {
  if (!context)
    return;

  DesktopActions *actions = static_cast<DesktopActions *>(context);
  actions->open_app("Browser");
}

inline void open_calculator(void *context) {
  if (!context)
    return;

  DesktopActions *actions = static_cast<DesktopActions *>(context);
  actions->open_app("Calculator");
}

inline void open_rock_ai(void *context) {
  if (!context)
    return;

  DesktopActions *actions = static_cast<DesktopActions *>(context);
  actions->open_app("Rock AI");
}

inline Action get_action(ContextMenuActions action) {
  switch (action) {
  case ContextMenuActions::OPEN_LAUNCHER:
    return open_launcher;

  case ContextMenuActions::NEXT_WALLPAPER:
    return next_wallpaper;

  case ContextMenuActions::OPEN_SETTINGS:
    return open_settings;

  case ContextMenuActions::OPEN_ABOUT:
    return open_about;

  case ContextMenuActions::MINIMIZE_WINDOW:
    return minimize_window;

  case ContextMenuActions::CLOSE_WINDOW:
    return close_window;

  case ContextMenuActions::REFRESH_DESKTOP:
    return refresh_desktop;

  case ContextMenuActions::TOGGLE_CURSOR:
    return toggle_cursor;

  case ContextMenuActions::TOGGLE_WALLPAPER:
    return toggle_wallpaper;

  case ContextMenuActions::OPEN_TERMINAL:
    return open_terminal;

  case ContextMenuActions::OPEN_FILE_MANAGER:
    return open_file_manager;

  case ContextMenuActions::OPEN_BROWSER:
    return open_browser;

  case ContextMenuActions::OPEN_CALCULATOR:
    return open_calculator;

  case ContextMenuActions::OPEN_ROCK_AI:
    return open_rock_ai;

  case ContextMenuActions::NONE:
  default:
    return nullptr;
  }
}
