#pragma once

#include "../../containers/string.hpp"

class ICliApp {
public:
  virtual const char *name() const = 0;

  virtual const char *quit_keyword() const { return "q"; }

  virtual void on_enter() {}

  virtual bool handle_input(int argc, char **argv) { return true; }

  virtual void on_exit() {}

  /* Virtual hooks for apps that want raw keystrokes instead of lines */
  virtual bool wants_raw_keys() const { return false; }

  virtual bool handle_key(char c) { return true; }

  virtual ~ICliApp() = default;
};
