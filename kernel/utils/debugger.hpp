#pragma once
#include "../storage/layout.hpp"

#include "terminal_utils.hpp"

namespace Debugger {
static void log(const char *str) {
  if (FS_DEBUG);
   // TerminalUtils::print(str);
}

static void log_number(int n) {
  if (FS_DEBUG);
    // TerminalUtils::print_number(n);
}

} // namespace Debugger
