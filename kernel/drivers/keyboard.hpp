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
  static constexpr u16 DATA_PORT = 0x60;
  static constexpr u16 STATUS_PORT = 0x64;

  static constexpr u8 RELEASE_MASK = 0x80;
  static constexpr u8 EXTENDED_PREFIX = 0xE0;

  static constexpr u8 ESCAPE = 0x01;
  static constexpr u8 BACKSPACE = 0x0E;
  static constexpr u8 ENTER = 0x1C;
  static constexpr u8 CAPS_LOCK = 0x3A;

  static constexpr u8 LEFT_SHIFT = 0x2A;
  static constexpr u8 RIGHT_SHIFT = 0x36;

  static constexpr u8 CTRL = 0x1D;
  static constexpr u8 ALT = 0x38;

  static constexpr u8 ARROW_UP = 0x48;
  static constexpr u8 ARROW_DOWN = 0x50;
  static constexpr u8 ARROW_LEFT = 0x4B;
  static constexpr u8 ARROW_RIGHT = 0x4D;
  static constexpr u8 DELETE = 0x53;

  static constexpr u8 NUM_LOCK = 0x45;

  static constexpr u8 NUMPAD_7 = 0x47;
  static constexpr u8 NUMPAD_8 = 0x48;
  static constexpr u8 NUMPAD_9 = 0x49;
  static constexpr u8 NUMPAD_MINUS = 0x4A;
  static constexpr u8 NUMPAD_4 = 0x4B;
  static constexpr u8 NUMPAD_5 = 0x4C;
  static constexpr u8 NUMPAD_6 = 0x4D;
  static constexpr u8 NUMPAD_PLUS = 0x4E;
  static constexpr u8 NUMPAD_1 = 0x4F;
  static constexpr u8 NUMPAD_2 = 0x50;
  static constexpr u8 NUMPAD_3 = 0x51;
  static constexpr u8 NUMPAD_0 = 0x52;
  static constexpr u8 NUMPAD_DOT = 0x53;

  static constexpr u8 NUMPAD_DIVIDE = 0x35;
  static constexpr u8 NUMPAD_MULTIPLY = 0x37;

  static constexpr int RING_BUFFER_SIZE = 1024;

  inline static KeyEvent buffer[RING_BUFFER_SIZE];
  inline static int head = 0;
  inline static int tail = 0;

  inline static bool extended = false;

  inline static bool shift = false;
  inline static bool ctrl = false;
  inline static bool alt = false;
  inline static bool altgr = false;
  inline static bool caps_lock = false;
  inline static bool num_lock = false;

  static constexpr bool is_release(u8 scancode) {
    return scancode & RELEASE_MASK;
  }

  static constexpr u8 base_scancode(u8 scancode) {
    return scancode & ~RELEASE_MASK;
  }

  static constexpr bool is_numpad_key(u8 scancode) {
    switch (scancode) {
    case NUMPAD_0:
    case NUMPAD_1:
    case NUMPAD_2:
    case NUMPAD_3:
    case NUMPAD_4:
    case NUMPAD_5:
    case NUMPAD_6:
    case NUMPAD_7:
    case NUMPAD_8:
    case NUMPAD_9:
    case NUMPAD_DOT:
      return true;

    default:
      return false;
    }
  }

  static constexpr KeyEvent handle_numpad_key(u8 scancode) {
    if (!num_lock)
      return {KeyType::None, 0};

    switch (scancode) {
    case NUMPAD_0:
      return make_event('0', KeyType::Char);

    case NUMPAD_1:
      return make_event('1', KeyType::Char);

    case NUMPAD_2:
      return make_event('2', KeyType::Char);

    case NUMPAD_3:
      return make_event('3', KeyType::Char);

    case NUMPAD_4:
      return make_event('4', KeyType::Char);

    case NUMPAD_5:
      return make_event('5', KeyType::Char);

    case NUMPAD_6:
      return make_event('6', KeyType::Char);

    case NUMPAD_7:
      return make_event('7', KeyType::Char);

    case NUMPAD_8:
      return make_event('8', KeyType::Char);

    case NUMPAD_9:
      return make_event('9', KeyType::Char);

    case NUMPAD_DOT:
      return make_event('.', KeyType::Char);

    case NUMPAD_MINUS:
      return make_event('-', KeyType::Char);

    case NUMPAD_PLUS:
      return make_event('+', KeyType::Char);

    default:
      return {KeyType::None, 0};
    }
  }

  static constexpr void handle_modifier_press(u8 scancode) {
    if (scancode == LEFT_SHIFT || scancode == RIGHT_SHIFT) {
      shift = true;
      return;
    }

    if (scancode == CTRL) {
      ctrl = true;
      return;
    }

    if (scancode == ALT) {
      if (extended) {
        altgr = true;
        extended = false;
      } else {
        alt = true;
      }

      return;
    }

    if (scancode == NUM_LOCK) {
      num_lock = !num_lock;
      return;
    }

    if (scancode == CAPS_LOCK) {
      caps_lock = !caps_lock;
      return;
    }
  }

  static constexpr void handle_modifier_release(u8 scancode) {
    u8 released = base_scancode(scancode);

    if (extended) {
      if (released == CTRL) {
        ctrl = false;
      } else if (released == ALT) {
        altgr = false;
      }

      extended = false;
      return;
    }

    if (released == LEFT_SHIFT || released == RIGHT_SHIFT) {
      shift = false;
    } else if (released == CTRL) {
      ctrl = false;
    } else if (released == ALT) {
      alt = false;
    }
  }

  static char translate(u8 scancode) {
    static constexpr char normal_map[64] = {
        0,   27,   '&',  ' ', '"', '\'', '(', '-', ' ', '_', ' ', ' ', ')',
        '=', '\b', '\t', 'a', 'z', 'e',  'r', 't', 'y', 'u', 'i', 'o', 'p',
        '^', '$',  '\n', 0,   'q', 's',  'd', 'f', 'g', 'h', 'j', 'k', 'l',
        'm', ' ',  '`',  0,   '*', 'w',  'x', 'c', 'v', 'b', 'n', ',', ';',
        ':', '!',  0,    '*', 0,   ' ',  0,   0,   0,   0,   0,   0};

    static constexpr char shift_map[64] = {
        0,   27,   '1',  '2', '3', '4', '5', '6', '7', '8', '9', '0', '_',
        '+', '\b', '\t', 'A', 'Z', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
        '"', '*',  '\n', 0,   'Q', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L',
        'M', '%',  '~',  0,   '!', 'W', 'X', 'C', 'V', 'B', 'N', '?', '.',
        '/', 0,    0,    '*', 0,   ' ', 0,   0,   0,   0,   0,   0};

    static constexpr char altgr_map[64] = {
        0,   27,   '~',  '#', '{',  '[', '|', '`', '\\', '@', ']', '}', 0,
        0,   '\b', '\t', 'a', 'z',  'e', 'r', 't', 'y',  'u', 'i', 'o', 'p',
        '^', '$',  '\n', 0,   'q',  's', 'd', 'f', 'g',  'h', 'j', 'k', 'l',
        'm', 0,    '`',  0,   '\\', 'w', 'x', 'c', 'v',  'b', 'n', 0,   0,
        0,   0,    0,    '*', 0,    ' ', 0,   0,   0,    0,   0,   0};

    static constexpr char shift_altgr_map[64] = {
        0,   27,   '~',  '#', '{', '[', '|', '`', '\\', '@', ']', '}', 0,
        '+', '\b', '\t', 'A', 'Z', 'E', 'R', 'T', 'Y',  'U', 'I', 'O', 'P',
        '"', '*',  '\n', 0,   'Q', 'S', 'D', 'F', 'G',  'H', 'J', 'K', 'L',
        'M', 0,    '~',  0,   '|', 'W', 'X', 'C', 'V',  'B', 'N', '?', '.',
        '/', 0,    0,    '*', 0,   ' ', 0,   0,   0,    0,   0,   0};

    if (scancode >= 64)
      return 0;

    if (altgr && shift)
      return shift_altgr_map[scancode];

    if (altgr)
      return altgr_map[scancode];

    if (shift)
      return shift_map[scancode];

    return normal_map[scancode];
  }

  static constexpr void push(KeyEvent event) {
    int next_head = (head + 1) % RING_BUFFER_SIZE;

    if (next_head == tail)
      return;

    buffer[head] = event;
    head = next_head;
  }

  static constexpr KeyEvent make_event(char ascii, KeyType type) {
    return {type, static_cast<u8>(ascii)};
  }

  static constexpr KeyEvent handle_extended_key(u8 scancode) {
    extended = false;

    switch (scancode) {
    case DELETE:
      return make_event('\b', KeyType::BackSpace);

    case ARROW_UP:
      return make_event(KEY_ARROW_UP, KeyType::ArrowUp);

    case ARROW_DOWN:
      return make_event(KEY_ARROW_DOWN, KeyType::ArrowDown);

    case ARROW_LEFT:
      return make_event(KEY_ARROW_LEFT, KeyType::ArrowLeft);

    case ARROW_RIGHT:
      return make_event(KEY_ARROW_RIGHT, KeyType::ArrowRight);

    case NUMPAD_DIVIDE:
      return make_event('/', KeyType::Char);

    default:
      return {KeyType::None, 0};
    }
  }

  static constexpr KeyEvent handle_normal_key(u8 scancode) {
    if (scancode == ESCAPE)
      return make_event(27, KeyType::Escape);

    if (scancode == BACKSPACE)
      return make_event('\b', KeyType::BackSpace);

    if (scancode == ENTER)
      return make_event('\n', KeyType::Enter);

    if (scancode == NUMPAD_MULTIPLY)
      return make_event('*', KeyType::Char);

    char ascii = translate(scancode);

    if (ascii == 0)
      return {KeyType::None, 0};

    return make_event(ascii, KeyType::Char);
  }

public:
  static constexpr KeyEvent read() {
    if (tail == head)
      return {KeyType::None, 0};

    KeyEvent event = buffer[tail];
    tail = (tail + 1) % RING_BUFFER_SIZE;

    return event;
  }

  static inline void interrupt_handler() {
    if (!(Asm::inb(STATUS_PORT) & 1))
      return;

    u8 scancode = Asm::inb(DATA_PORT);

    Random::add_entropy(Timer::ticks ^ scancode);

    if (scancode == EXTENDED_PREFIX) {
      extended = true;
      return;
    }

    if (is_release(scancode)) {
      handle_modifier_release(scancode);
      return;
    }

    if (!extended) {
      if (scancode == LEFT_SHIFT || scancode == RIGHT_SHIFT ||
          scancode == CTRL || scancode == ALT || scancode == CAPS_LOCK) {
        handle_modifier_press(scancode);
        return;
      }
    }

    if (!extended && scancode == NUM_LOCK) {
      handle_modifier_press(scancode);
      return;
    }

    if (!extended && is_numpad_key(scancode)) {
      KeyEvent event = handle_numpad_key(scancode);

      if (event.keytype != KeyType::None)
        push(event);

      return;
    }

    KeyEvent event;

    if (extended)
      event = handle_extended_key(scancode);
    else
      event = handle_normal_key(scancode);

    if (event.keytype != KeyType::None)
      push(event);
  }

  static constexpr bool is_shift_down() { return shift; }

  static constexpr bool is_ctrl_down() { return ctrl; }

  static constexpr bool is_alt_down() { return alt; }

  static constexpr bool is_altgr_pressed() { return altgr; }

  static constexpr bool is_caps_lock_pressed() { return caps_lock; }

  static constexpr bool is_num_lock_pressed() { return num_lock; }
};

extern "C" void c_keyboard_handler() { Keyboard::interrupt_handler(); }
