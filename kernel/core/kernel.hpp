#pragma once

#include "timer.hpp"
#include <stdarg.h>

namespace Kernel {
inline int vram_index = 0;
inline volatile char *video_memory = (volatile char *)0xB8000;

inline void reboot() {
  // Pulse the CPU reset line via the keyboard controller
  Asm::outb(keyboard_command_port, reboot_data);
}

inline void halt() { Asm::halt(); }

} // namespace Kernel
