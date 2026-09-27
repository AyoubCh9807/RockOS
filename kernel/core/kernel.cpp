#include "kernel.hpp"

#include "../../boot/multiboot2.hpp"
#include "../desktop/desktop.hpp"
#include "../gui/window_app_registry.hpp"
#include "../gui/window_manager.hpp"
#include "../memory/heap.hpp"
// #include "../process/process_manager.hpp"
// #include "../process/scheduler.hpp"
#include "../random/random.hpp"
#include "../shared/types.hpp"
// #include "../shell/shell.hpp"
#include "../drivers/mouse.hpp"
#include "../shell/terminal.hpp"
#include "../shell/terminal_registry.hpp"
#include "../utils/debugger.hpp"
#include "../utils/math_utils.hpp"

#include "crti.hpp"
#include "timer.hpp"

extern "C" void c_pagefault_handler(u64 *saved_regs) { (void)saved_regs; }

extern "C" void c_gpfault_handler(u64 *saved_regs) { (void)saved_regs; }
extern "C" void kernel_main(u64 mb_addr) {
  call_constructors();

  Multiboot2::fill_tags(mb_addr);

  Debugger::log(Debugger::DebugType::KERNEL, "KERNEL MAIN ENTERED\n");

  heap.init_heap();

  Timer::init();
  Random::init();

  Mouse::init();

  Mouse::set_coords(Multiboot2::framebuffer.width / 2,
                    Multiboot2::framebuffer.height / 2);

  Asm::sti();

  Disk disk;
  FileSystem fs(disk);

  if (!fs.mount()) {
    Debugger::log(Debugger::DebugType::FS, "No filesystem, formatting...\n");

    if (!fs.format()) {
      Debugger::log(Debugger::DebugType::FS,
                    "FORMAT FAILED - filesystem commands will not work\n");
    }
  } else {
    Debugger::log(Debugger::DebugType::FS, "MOUNT SUCCESS\n");
  }

  u32 current_dir = ROOT_INODE;

  TerminalUtils terminal_utils;

  Environment env(terminal_utils);

  Vocabulary ai_vocab;
  RockAI ai(ai_vocab);

  TerminalRegistry terminal_registry(terminal_utils, fs, current_dir, env, ai);

  CliAppRegistry cli_app_registry(terminal_utils);

  Terminal terminal(terminal_utils, fs, terminal_registry, cli_app_registry,
                    env);

  terminal.fill_registry();

  ShellHistory shell_history;
  Shell shell(terminal, shell_history);

  WindowManager wm;

  WindowAppRegistry window_app_reg(terminal, shell);

  DialogManager dialog_manager;

  AppLauncher app_launcher(wm, window_app_reg, dialog_manager);

  NotificationService notification_service;

  WidgetRegistry widget_registry;
  WidgetManager widget_manager(widget_registry);

  Desktop desktop(wm, window_app_reg, dialog_manager, app_launcher,
                  notification_service, widget_manager);

  window_app_reg.set_desktop_actions(desktop);
  window_app_reg.fill_registry();

  widget_registry.register_widgets();

  const char *app_names[] = {
      "Counter",  "Dice",      "DVD",      "Clock", "Tyrant",  "Matrix",
      "About",    "Settings",

      "Rock AI",  "Files",     "Terminal", "Amp",   "Tuner",   "Metronome",
      "Playlist", "Radio",     "Lyrics",   "Mixer", "Browser", "Rock Store",
      "Vinyl",    "Recorder",  "Drums",    "REC",   "Lock",    "Trash",
      "Updater",  "Equalizer", "Pick",     "Help",  "Stage"};

  const u32 screen_width = Multiboot2::framebuffer.width;

  constexpr u32 MARGIN = 20;

  // App icon layout (left side of the screen)

  constexpr u32 APP_SPACING_X = 75;
  constexpr u32 APP_SPACING_Y = 75;

  // Apps live in roughly the left 40% of the screen, but the section
  // never shrinks below room for at least two columns even on a
  // narrow framebuffer.
  const u32 apps_area_width =
      MathUtils::max(APP_SPACING_X * 2, screen_width * 40 / 100);

  const u32 app_columns = MathUtils::max(1u, apps_area_width / APP_SPACING_X);

  u32 app_x = MARGIN;
  u32 app_y = MARGIN;

  u32 app_column = 0;

  for (const char *name : app_names) {
    desktop.add_icon(name, app_x, app_y);

    app_column++;

    if (app_column >= app_columns) {
      app_column = 0;

      app_x = MARGIN;
      app_y += APP_SPACING_Y;
    } else {
      app_x += APP_SPACING_X;
    }
  }

  // Widget masonry layout (right side of the screen)

  constexpr u32 WIDGET_GAP = 16;
  constexpr u32 MAX_WIDGET_COLUMNS = 8;

  const u32 widget_start_x = MARGIN + apps_area_width + MARGIN;
  const u32 widget_start_y = MARGIN;

  const u32 widget_area_right =
      screen_width > MARGIN ? screen_width - MARGIN : widget_start_x;

  const int widget_count = widget_registry.get_count();

  // Size columns to the widest widget actually registered, so the
  // layout adapts to whatever widgets exist instead of assuming a
  // fixed size.
  u32 max_widget_width = 0;

  for (int i = 0; i < widget_count; i++) {
    IWidget *widget = widget_registry.get(i);

    if (!widget)
      continue;

    max_widget_width = MathUtils::max(max_widget_width, widget->get_width());
  }

  if (max_widget_width == 0)
    max_widget_width = 200;

  const u32 col_width = max_widget_width + WIDGET_GAP;

  const u32 widget_area_width = widget_area_right > widget_start_x
                                    ? widget_area_right - widget_start_x
                                    : col_width;

  u32 widget_columns = MathUtils::max(1u, widget_area_width / col_width);
  widget_columns = MathUtils::min(widget_columns, MAX_WIDGET_COLUMNS);

  u32 column_x[MAX_WIDGET_COLUMNS];
  u32 column_y[MAX_WIDGET_COLUMNS];

  for (u32 c = 0; c < widget_columns; c++) {
    column_x[c] = widget_start_x + c * col_width;
    column_y[c] = widget_start_y;
  }

  //NOTE: Ai is used for UI UX design
  // Standard "shortest column" masonry: each widget goes into
  // whichever column currently has the least height used so far.
  // This stays balanced and correct no matter how many widgets are
  // registered or how tall each one turns out to be.
  for (int i = 0; i < widget_count; i++) {
    IWidget *widget = widget_registry.get(i);

    if (!widget)
      continue;

    u32 shortest_column = 0;

    for (u32 c = 1; c < widget_columns; c++) {
      if (column_y[c] < column_y[shortest_column])
        shortest_column = c;
    }

    widget->set_position(column_x[shortest_column],
                         column_y[shortest_column]);

    column_y[shortest_column] += widget->get_height() + WIDGET_GAP;
  }

  desktop.init();

  desktop.run();
}
