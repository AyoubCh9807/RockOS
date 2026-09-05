#pragma once

#include "../core/asm.hpp"
#include "../core/timer.hpp"
#include "../random/random.hpp"
#include "../shared/key_event.hpp"
#include "../shared/types.hpp"
#include "../utils/string_utils.hpp"
#include "../utils/terminal_utils.hpp"

class Keyboard {
private:
  static constexpr u16 KEYBOARD_PORT = 0x60;
  static constexpr u16 KEYBOARD_STATUS_PORT = 0x64;
  static constexpr unsigned char RELEASE_MASK = 0x80;
  static constexpr unsigned char BACKSPACE_SCANCODE = 0x0E;
  static constexpr unsigned char ENTER_SCANCODE = 0x1C;
  static constexpr unsigned char SPECIAL_KEY_SCANCODE = 0xE0;
  static constexpr unsigned char DELETE_SCANCODE = 0x53;
  static constexpr int KEYBOARD_RING_BUFFER_SIZE = 1024;
  static constexpr unsigned char ARROW_UP_SCANCODE = 0x48;
  static constexpr unsigned char ARROW_DOWN_SCANCODE = 0x50;
  static constexpr unsigned char ARROW_LEFT_SCANCODE = 0x4B;
  static constexpr unsigned char ARROW_RIGHT_SCANCODE = 0x4D;
  static constexpr unsigned char CTRL_SCANCODE = 0x1D;
  static constexpr unsigned char ESCAPE_SCANCODE = 0x01;
  // Circular buffer implementation (must be static for header-only classes)
  inline static KeyEvent buffer[KEYBOARD_RING_BUFFER_SIZE];
  inline static int head = 0;
  inline static int tail = 0;
  inline static bool special_extended = false;

  inline static bool is_shift = false;
  inline static bool is_ctrl = false;
  inline static bool is_altgr = false;
  inline static bool is_caps_lock = false;

public:
  static char translate(unsigned char scancode) {

    // We only use the first 64 Set 1 scancodes here.
    // Extended keys such as arrows and Delete are handled separately.
    static const char normal_map[64] = {
        0,   27,   '&',  ' ', '"', '\'', '(', '-', ' ', '_', ' ', ' ', ')',
        '=', '\b', '\t', 'a', 'z', 'e',  'r', 't', 'y', 'u', 'i', 'o', 'p',
        '^', '$',  '\n', 0,   'q', 's',  'd', 'f', 'g', 'h', 'j', 'k', 'l',
        'm', ' ',  '`',  0,   '*', 'w',  'x', 'c', 'v', 'b', 'n', ',', ';',
        ':', '!',  0,    '*', 0,   ' ',  0,   0,   0,   0,   0,   0};

    static const char shift_map[64] = {
        0,   27,   '1',  '2', '3', '4', '5', '6', '7', '8', '9', '0', '_',
        '+', '\b', '\t', 'A', 'Z', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
        '"', '*',  '\n', 0,   'Q', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L',
        'M', '%',  '~',  0,   '!', 'W', 'X', 'C', 'V', 'B', 'N', '?', '.',
        '/', 0,    0,    '*', 0,   ' ', 0,   0,   0,   0,   0,   0};

    static const char altgr_map[64] = {
        0,   27,   '~',  '#', '{',  '[', '|', '`', '\\', '@', ']', '}', 0,
        0,   '\b', '\t', 'a', 'z',  'e', 'r', 't', 'y',  'u', 'i', 'o', 'p',
        '^', '$',  '\n', 0,   'q',  's', 'd', 'f', 'g',  'h', 'j', 'k', 'l',
        'm', 0,    '`',  0,   '\\', 'w', 'x', 'c', 'v',  'b', 'n', 0,   0,
        0,   0,    0,    '*', 0,    ' ', 0,   0,   0,    0,   0,   0};

    static const char shift_altgr_map[64] = {
        0,   27,   '~',  '#', '{', '[', '|', '`', '\\', '@', ']', '}', 0,
        '+', '\b', '\t', 'A', 'Z', 'E', 'R', 'T', 'Y',  'U', 'I', 'O', 'P',
        '"', '*',  '\n', 0,   'Q', 'S', 'D', 'F', 'G',  'H', 'J', 'K', 'L',
        'M', 0,    '~',  0,   '|', 'W', 'X', 'C', 'V',  'B', 'N', '?', '.',
        '/', 0,    0,    '*', 0,   ' ', 0,   0,   0,    0,   0,   0};

    if (scancode >= 64)
      return 0;

    if (is_altgr && is_shift)
      return shift_altgr_map[scancode];

    if (is_altgr)
      return altgr_map[scancode];

    if (is_shift)
      return shift_map[scancode];

    return normal_map[scancode];
  }

  static void push(KeyEvent ev) {
    int next_head = (head + 1) % KEYBOARD_RING_BUFFER_SIZE;
    if (next_head != tail) {
      buffer[head] = ev;
      head = next_head;
    }
  }

  static KeyEvent read() {
    if (tail == head)
      return {KeyType::None, 0};
    KeyEvent ev = buffer[tail];
    tail = (tail + 1) % KEYBOARD_RING_BUFFER_SIZE;
    return ev;
  }

  // Returns an ASCII character, or special codes, or 0 if no key was pressed.
  inline static void interrupt_handler() {
    if (!(Asm::inb(KEYBOARD_STATUS_PORT) & 1))
      return;

    KeyEvent ev;
    KeyType ktype = KeyType::None;

    unsigned char scancode = Asm::inb(KEYBOARD_PORT);

    // Use keyboard input for randomness.
    Random::add_entropy(Timer::ticks ^ scancode);

    // EXTENDED SCANCODE PREFIX

    if (scancode == SPECIAL_KEY_SCANCODE) {
      special_extended = true;
      return;
    }

    // KEY RELEASE

    if (scancode & RELEASE_MASK) {
      unsigned char released = scancode & ~RELEASE_MASK;

      // Normal modifier releases.
      if (released == 0x2A || released == 0x36) {
        is_shift = false;
      } else if (released == CTRL_SCANCODE) {
        is_ctrl = false;
      } else if (released == 0x38) {
        is_altgr = false;
      }

      // Extended modifier releases.
      // Right Ctrl = E0 9D
      // Right Alt  = E0 B8
      // I will put the bytes in variables some other time
      if (special_extended) {
        if (released == 0x1D) {
          is_ctrl = false;
        } else if (released == 0x38) {
          is_altgr = false;
        }
      }

      special_extended = false;
      return;
    }

    // KEY PRESS
    // Normal modifier presses.
    if (scancode == 0x2A || scancode == 0x36) {
      is_shift = true;
      return;
    }

    if (scancode == CTRL_SCANCODE) {
      is_ctrl = true;
      return;
    }

    if (scancode == 0x38) {
      is_altgr = true;
      return;
    }

    // Caps Lock toggles.
    if (scancode == 0x3A) {
      is_caps_lock = !is_caps_lock;
      return;
    }

    // EXTENDED KEYS

    char ascii = 0;

    if (special_extended) {

      if (scancode == DELETE_SCANCODE) {
        ascii = '\b';
        ktype = KeyType::BackSpace;
      }

      else if (scancode == ARROW_UP_SCANCODE) {
        ascii = KEY_ARROW_UP;
        ktype = KeyType::ArrowUp;
      }

      else if (scancode == ARROW_DOWN_SCANCODE) {
        ascii = KEY_ARROW_DOWN;
        ktype = KeyType::ArrowDown;
      }

      else if (scancode == ARROW_LEFT_SCANCODE) {
        ascii = KEY_ARROW_LEFT;
        ktype = KeyType::ArrowLeft;
      }

      else if (scancode == ARROW_RIGHT_SCANCODE) {
        ascii = KEY_ARROW_RIGHT;
        ktype = KeyType::ArrowRight;
      }

      special_extended = false;
    }

    // NORMAL KEYS

    else {

      if (scancode == ESCAPE_SCANCODE) {
        ascii = 27;
        ktype = KeyType::Escape;
      }
      if (scancode == BACKSPACE_SCANCODE) {
        ascii = '\b';
        ktype = KeyType::BackSpace;
      }

      else if (scancode == ENTER_SCANCODE) {
        ascii = '\n';
        ktype = KeyType::Enter;
      }

      else {
        ascii = translate(scancode);
        ktype = KeyType::Char;
      }
    }

    ev.scancode = scancode;
    ev.keytype = ktype;

    // PUSH EVENT

    if (ascii != 0) {
      ev.scancode = ascii;
      ev.keytype = ktype;

      push(ev);
    }
  }
};

extern "C" void c_keyboard_handler() { Keyboard::interrupt_handler(); }
