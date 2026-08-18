#pragma once

#include "timer.hpp"
#include <stdarg.h>
#include "asm.hpp"

namespace Kernel {

inline void reboot() {
  // Pulse the CPU reset line via the keyboard controller
  Asm::outb(keyboard_command_port, reboot_data);
}

inline void halt() {
  Timer::set_idle(false);
  Asm::halt();
  Timer::set_idle(true);

}

} // namespace Kernel
