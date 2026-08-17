#pragma once
#include "../storage/layout.hpp"
#include "terminal_utils.hpp"

class Debugger {

public:
  static void log(const char *str) {
    if (FS_DEBUG)
      TerminalUtils::static_print(str, 0xFFFFFF);
  }

  static void log_number(int n) {
    if (FS_DEBUG)
      TerminalUtils::static_print(StringUtils::iota(n), 0xFFFFFF);
  }

  /*
   static void log_number(int n) {
      if (FS_DEBUG) {
          volatile int x = 0;
          x++;
      }
  }
  static void log(const char *str) {
      if (FS_DEBUG) {
          volatile int x = 0;
          x++;
      }
  }
  */
};
