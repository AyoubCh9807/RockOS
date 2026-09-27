#pragma once
class AppLauncher;

class DesktopActions {
public:
  virtual void open_app(const char *name) = 0;
  virtual void close_focused_window() = 0;
  virtual void minimize_focused_window() = 0;
  virtual void next_wallpaper() = 0;
  virtual void open_launcher() = 0;
  virtual AppLauncher &get_app_launcher() = 0;

  virtual ~DesktopActions() = default;
};
