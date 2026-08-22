#pragma once
#include "../storage/layout.hpp"
#include "terminal_utils.hpp"

class Debugger {
public:
  static void log(const char *str) {
    if (FS_DEBUG)
      TerminalUtils::static_print(str, 0xFFFFFF);
  }

  static void logf(const char *fmt, ...) {
    if (!FS_DEBUG)
      return;
    char buf[256];
    va_list args;
    va_start(args, fmt);
    StringUtils::vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    TerminalUtils::static_print(buf, 0xFFFFFF);
  }

  static void log_number(int n) {
    if (!FS_DEBUG)
      return;
    char buf[16];
    StringUtils::snprintf(buf, sizeof(buf), "%d", n);
    TerminalUtils::static_print(buf, 0xFFFFFF);
  }
};
