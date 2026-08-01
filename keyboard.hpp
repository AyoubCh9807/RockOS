#pragma once

#include "utils/string_utils.hpp"
#include "asm.hpp"
#include "shared/types.hpp"

class Keyboard {
private:
    static constexpr u16 KEYBOARD_PORT = 0x60;
    static constexpr u16 KEYBOARD_STATUS_PORT = 0x64;
    static constexpr unsigned char RELEASE_MASK = 0x80;
    static constexpr unsigned char BACKSPACE_SCANCODE = 0x0E;
    static constexpr unsigned char ENTER_SCANCODE = 0x1C;
    static constexpr unsigned char SPECIAL_KEY_SCANCODE = 0xE0;
    static constexpr unsigned char DELETE_SCANCODE = 0x53;

public:
    // Returns an ASCII character, or special codes, or 0 if no key was pressed.
    char poll() {
        if (Asm::inb(KEYBOARD_STATUS_PORT) & 1) {
            unsigned char scancode = Asm::inb(KEYBOARD_PORT);

            if (scancode & RELEASE_MASK)
                return 0;

            if (scancode == BACKSPACE_SCANCODE)
                return '\b';

            if (scancode == ENTER_SCANCODE)
                return '\n';

            if (scancode == SPECIAL_KEY_SCANCODE) {
                int next_scancode = Asm::inb(KEYBOARD_PORT);
                if (next_scancode & RELEASE_MASK)
                    return 0;

                if (next_scancode == DELETE_SCANCODE)
                    return '\b'; // Or map delete to a special code if you want
                
                return 0;
            }

            return StringUtils::scancode_to_ascii(scancode);
        }
        return 0;
    }
};
