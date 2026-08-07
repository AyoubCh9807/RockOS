#pragma once

#include "../core/kernel.hpp"
#include "string_utils.hpp"

namespace TerminalUtils {

inline static void clear() {
	for (int i = 0; i < 80 * 25 * 2; i += 2) {
		Kernel::video_memory[i] = ' ';
		Kernel::video_memory[i + 1] = 0x0F;
	}
	Kernel::vram_index = 0;
}

inline static void putchar(char c) {
	if (c == '\n') {
		Kernel::vram_index = ((Kernel::vram_index + 160) / 160) * 160;
	} else if (c == '\b') {
		if (Kernel::vram_index > 0) {
			Kernel::vram_index -= 2;
			Kernel::video_memory[Kernel::vram_index] = ' ';
			Kernel::video_memory[Kernel::vram_index + 1] = 0x0F;
		}
	} else {
		Kernel::video_memory[Kernel::vram_index] = c;
		Kernel::video_memory[Kernel::vram_index + 1] = 0x0F;
		Kernel::vram_index += 2;
	}

	// Prevent running off the VGA text buffer.
	if (Kernel::vram_index >= 80 * 25 * 2) {
		Kernel::vram_index = 0; // simple wrap (until you implement scrolling)
	}
}


inline static void print(const char *str) {
	if (!str) return;
	for (int i = 0; str[i] != '\0'; i++) {
		putchar(str[i]);
	}
}


inline static void print_number(int n) {
  print(StringUtils::iota(n));
}

} // namespace TerminalUtils
