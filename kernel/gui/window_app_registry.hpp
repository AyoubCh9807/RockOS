#pragma once

#include "../utils/string_utils.hpp"

#include "apps/clock_app.hpp"
#include "apps/counter_app.hpp"
#include "apps/dice_app.hpp"
#include "apps/dvd_app.hpp"
#include "apps/matrix_app.hpp"
#include "apps/tyrant_app.hpp"
#include "apps/about_app.hpp"

#include "window_app.hpp"

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

  int count = 0;

public:
  WindowAppRegistry() : counter(), tyrant(), clock(), dice(), matrix(), dvd(), about() {}

  void register_app(IWindowApp *app) {
    if (count >= MAX_WINDOW_APPS)
      return;

    apps[count++] = app;
  }

  void fill_registry() {
    register_app(&counter);
    register_app(&tyrant);
    register_app(&clock);
    register_app(&dice);
    register_app(&matrix);
    register_app(&dvd);
    register_app(&about);
  }


  IWindowApp *find(const char *name) {
    for (int i = 0; i < count; i++) {
      if (StringUtils::strcmp(name, apps[i]->name()) == 0)
        return apps[i];
    }

    return nullptr;
  }
};
