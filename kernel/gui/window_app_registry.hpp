#pragma once

#include "../utils/string_utils.hpp"

#include "apps/about_app.hpp"
#include "apps/browser_app.hpp"
#include "apps/calculator_app.hpp"
#include "apps/clock_app.hpp"
#include "apps/counter_app.hpp"
#include "apps/dice_app.hpp"
#include "apps/dvd_app.hpp"
#include "apps/matrix_app.hpp"
#include "apps/rock_ai_app.hpp"
#include "apps/settings_app.hpp"
#include "apps/terminal_app.hpp"
#include "apps/tyrant_app.hpp"

#include "window_app.hpp"
#include "window_manager.hpp"

constexpr int MAX_WINDOW_APPS = 256;

class WindowAppRegistry {
private:
  IWindowApp *apps[MAX_WINDOW_APPS]{};

  CounterApp counter;
  TyrantApp tyrant;
  ClockApp clock;
  DiceApp dice;
  MatrixApp matrix;
  DvdApp dvd;
  AboutApp about;
  SettingsApp settings;
  RockAIApp rock_ai;
  TerminalApp terminal;
  CalculatorApp calculator;
  BrowserApp browser;

  int count = 0;

public:
  WindowAppRegistry(Terminal &term, Shell &shell)
      : counter(), tyrant(), clock(), dice(), matrix(), dvd(), about(),
        settings(), rock_ai(), terminal(term, shell), calculator(), browser() {}

  void register_app(IWindowApp *app) {
    if (count >= MAX_WINDOW_APPS)
      return;

    apps[count++] = app;
  }

  void set_desktop_actions(DesktopActions &actions) {
    rock_ai.set_desktop_actions(actions);
  }

  void fill_registry() {
    register_app(&counter);
    register_app(&tyrant);
    register_app(&clock);
    register_app(&dice);
    register_app(&matrix);
    register_app(&dvd);
    register_app(&about);
    register_app(&settings);
    register_app(&rock_ai);
    register_app(&terminal);
    register_app(&calculator);
    register_app(&browser);
  }

  IWindowApp *find(const char *name) {
    for (int i = 0; i < count; i++) {
      if (StringUtils::strcmp(name, apps[i]->name()) == 0)
        return apps[i];
    }

    return nullptr;
  }

  void launch_app(const char *label, WindowManager &window_manager) {
    IWindowApp *app = find(label);

    if (!app)
      return;

    window_manager.create_window(app, 0, 0, 640, 480);
  }
};
