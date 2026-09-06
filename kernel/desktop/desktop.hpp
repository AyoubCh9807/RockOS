#pragma once

#include "../../boot/graphics.hpp"

#include "../gui/dialog_manager.hpp"
#include "../gui/window_app_registry.hpp"
#include "../gui/window_manager.hpp"

#include "desktop_icon.hpp"
#include "wallpaper.hpp"

#include "../drivers/mouse.hpp"

static constexpr auto MAX_DESKTOP_APPS = 256;
static constexpr auto DEFAULT_WINDOW_WIDTH = 600;
static constexpr auto DEFAULT_WINDOW_HEIGHT = 480;

class Desktop {
private:
  WindowManager &window_manager;
  WindowAppRegistry &window_app_registry;
  DialogManager &dialog_manager;

  DesktopIcon icons[MAX_DESKTOP_APPS];

  int icon_count = 0;
  int selected_icon = 0;

  u32 background_color = Colors::DARK_RED;

  int LAUNCH_X = 0;
  int LAUNCH_Y = 0;
  int LAUNCH_DX = 32;
  int LAUNCH_DY = 32;

  inline static auto SCREEN_WIDTH = Multiboot2::framebuffer.width;

  inline static auto SCREEN_HEIGHT = Multiboot2::framebuffer.height;

  void draw_background() { Graphics::clear(background_color); }

  void handle_launch_coords() {
    int MAX_X = SCREEN_WIDTH - DEFAULT_WINDOW_WIDTH;
    int MAX_Y = SCREEN_HEIGHT - DEFAULT_WINDOW_HEIGHT;

    int next_x = LAUNCH_X + LAUNCH_DX;
    int next_y = LAUNCH_Y + LAUNCH_DY;

    if (next_x < 0 || next_x > MAX_X) {
      LAUNCH_DX = -LAUNCH_DX;
      next_x = LAUNCH_X + LAUNCH_DX;
    }

    if (next_y < 0 || next_y > MAX_Y) {
      LAUNCH_DY = -LAUNCH_DY;
      next_y = LAUNCH_Y + LAUNCH_DY;
    }

    LAUNCH_X = next_x;
    LAUNCH_Y = next_y;
  }

  void draw_mouse_debug() {
    char debug[128];

    StringUtils::snprintf(debug, sizeof(debug), "Mouse IRQ: %u",
                          (unsigned)Mouse::get_irq_count());

    Graphics::draw_string(debug, 10, 10, Colors::pick_random_color());

    StringUtils::snprintf(debug, sizeof(debug), "Last byte: %x",
                          (unsigned)Mouse::get_last_byte());

    Graphics::draw_string(debug, 10, 30, Colors::WHITE);

    StringUtils::snprintf(debug, sizeof(debug), "Packet: %x %x %x",
                          (unsigned)Mouse::get_packet_byte(0),
                          (unsigned)Mouse::get_packet_byte(1),
                          (unsigned)Mouse::get_packet_byte(2));

    Graphics::draw_string(debug, 10, 50, Colors::WHITE);

    StringUtils::snprintf(debug, sizeof(debug), "Mouse: %d %d", Mouse::get_x(),
                          Mouse::get_y());

    Graphics::draw_string(debug, 10, 70, Colors::WHITE);
  }

public:
  Desktop(WindowManager &wm, WindowAppRegistry &window_app_registry,
          DialogManager &dialog_manager)
      : window_manager(wm), window_app_registry(window_app_registry),
        dialog_manager(dialog_manager) {}

  void init() {
    clear();

    Dialog *dialog = dialog_manager.create_dialog(
        200, 150, 400, 200, "Welcome to Rock OS", "Welcome! Enjoy your stay.");

    if (dialog)
      dialog_manager.show(dialog);
  }

  void update() {
    KeyEvent ev = Keyboard::read();

    if (ev.scancode != 0 && ev.keytype != KeyType::None) {

      if (dialog_manager.has_active()) {

        // Key event gets routed to the dialog manager.
        dialog_manager.route_key(ev);

      } else if (handle_key(ev)) {

        // Desktop handled it.

      } else {

        // Key event gets routed to the window manager.
        window_manager.route_key(ev);
      }
    }

    window_manager.update();
  }

  void render() {
    draw_background();

    Wallpaper::draw_selected_wallpaper();

    draw_icons();

    window_manager.render();

    dialog_manager.render();

    draw_taskbar();

    Graphics::draw_cursor(Mouse::get_x(), Mouse::get_y());

    // Temporary mouse diagnostics.
    // Remove this once mouse movement works.
    draw_mouse_debug();

    Graphics::present();
  }

  void clear() { draw_background(); }

  void draw_taskbar() {
    u32 width = Multiboot2::framebuffer.width;
    u32 height = Multiboot2::framebuffer.height;

    constexpr u32 TASKBAR_HEIGHT = 64;

    u32 taskbar_y = height - TASKBAR_HEIGHT;

    // Taskbar.
    Graphics::draw_rect(0, taskbar_y, width, TASKBAR_HEIGHT, Colors::DARK_GRAY);

    // Top border.
    Graphics::draw_rect(0, taskbar_y, width, 2, Colors::BLUE);

    // Rock OS start button.
    constexpr u32 START_X = 12;
    constexpr u32 START_SIZE = 44;

    Graphics::draw_rect(START_X, taskbar_y + 10, START_SIZE, START_SIZE,
                        Colors::BLUE);

    Graphics::draw_string("R", START_X + 17, taskbar_y + 28, Colors::WHITE);

    // Mocked app buttons.
    constexpr u32 APP_START_X = 72;
    constexpr u32 APP_SIZE = 44;
    constexpr u32 APP_GAP = 8;

    const char *apps[] = {
        "C", "D", "P", "M", "T",
    };

    constexpr u32 APP_COUNT = 5;

    for (u32 i = 0; i < APP_COUNT; i++) {
      u32 x = APP_START_X + i * (APP_SIZE + APP_GAP);

      Graphics::draw_rect(x, taskbar_y + 10, APP_SIZE, APP_SIZE, Colors::GRAY);

      Graphics::draw_string(apps[i], x + 17, taskbar_y + 28, Colors::WHITE);
    }

    // System information.
    constexpr u32 INFO_X = 380;

    u32 used_memory = heap.get_used();

    char memory_text[32];

    if (used_memory > 1024 * 1024) {

      StringUtils::snprintf(memory_text, sizeof(memory_text), "MEM %u.%u MB",
                            used_memory / (1024 * 1024),
                            (used_memory % (1024 * 1024)) / 102400);

    } else {

      StringUtils::snprintf(memory_text, sizeof(memory_text), "MEM %u KB",
                            used_memory / 1024);
    }

    u32 memory_text_color = heap.memory_critical()    ? Colors::RED
                            : heap.memory_dangerous() ? Colors::ORANGE
                            : heap.memory_high()      ? Colors::GOLD
                            : heap.memory_medium()    ? Colors::YELLOW
                            : heap.memory_low()       ? Colors::GREEN
                                                      : Colors::WHITE;

    Graphics::draw_string(memory_text, INFO_X, taskbar_y + 18,
                          memory_text_color);

    // Uptime.
    char uptime_text[32];

    Timer::get_formatted_time_into(uptime_text, sizeof(uptime_text));

    Graphics::draw_string(uptime_text, INFO_X, taskbar_y + 38, Colors::WHITE);

    // RTC date.
    u32 year = RTC::get_year() + 2000;

    u8 month = RTC::get_month();

    u8 day = RTC::get_day();

    char date_text[32];

    StringUtils::snprintf(date_text, sizeof(date_text), "%d-%d-%d", (int)year,
                          (int)month, (int)day);

    // Clock.
    u8 hour = RTC::get_hours() % 24;

    u8 minute = RTC::get_minutes() % 60;

    u8 second = RTC::get_seconds() % 60;

    char clock_text[16];

    StringUtils::snprintf(clock_text, sizeof(clock_text), "%02d:%02d:%02d",
                          (int)hour, (int)minute, (int)second);

    Graphics::draw_string(date_text, width - 160, taskbar_y + 14,
                          Colors::WHITE);

    Graphics::draw_string(clock_text, width - 80, taskbar_y + 34,
                          Colors::WHITE);
  }

  void draw_icons() {
    for (int i = 0; i < icon_count; i++) {

      if (selected_icon == i)
        icons[i].select();
      else
        icons[i].unselect();

      icons[i].draw();
    }
  }

  void add_icon(const char *label, u32 x, u32 y) {

    if (icon_count >= MAX_DESKTOP_APPS)
      return;

    icons[icon_count++] = DesktopIcon(label, x, y);
  }

  void launch_app(DesktopIcon &icon) {
    IWindowApp *app = window_app_registry.find(icon.get_label());

    if (!app)
      return;

    Window *window = window_manager.create_window(
        app, LAUNCH_X, LAUNCH_Y, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);

    if (!window)
      return;

    handle_launch_coords();

    if (heap.memory_high() && !dialog_manager.has_active()) {

      Dialog *dialog =
          dialog_manager.create_dialog(200, 150, 400, 200, "Memory Warning",
                                       "Memory usage is getting high!");

      if (dialog)
        dialog_manager.show(dialog);
    }
  }

  void select_next_icon() {
    if (icon_count == 0)
      return;

    selected_icon = (selected_icon + 1) % icon_count;
  }

  void select_prev_icon() {
    if (icon_count == 0)
      return;

    if (selected_icon == 0) {
      selected_icon = icon_count - 1;
      return;
    }

    selected_icon--;
  }

  bool handle_key(const KeyEvent &ev) {

    if (ev.keytype == KeyType::ArrowRight) {
      select_next_icon();
      return true;
    }

    if (ev.keytype == KeyType::ArrowLeft) {
      select_prev_icon();
      return true;
    }

    if (ev.keytype == KeyType::Enter ||
        (ev.keytype == KeyType::Char && ev.scancode == 'o')) {

      if (icon_count > 0)
        launch_app(icons[selected_icon]);

      return true;
    }

    if (ev.keytype == KeyType::Char && ev.scancode == 'w') {
      Wallpaper::select_next_wallpaper();
      return true;
    }

    return false;
  }

  void run() {
    u32 last_frame_tick = Timer::get_ticks();

    while (1) {

      update();

      u32 now = Timer::get_ticks();

      if (now - last_frame_tick >= FRAME_TICKS) {

        last_frame_tick = now;

        render();
      }

      Kernel::halt();
    }
  }
};
