#pragma once

#include "asm.hpp"
#include "shared/key_event.hpp"
#include "shared/types.hpp"
#include "utils/string_utils.hpp"

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

  // Circular buffer implementation (must be static for header-only classes)
  inline static KeyEvent buffer[KEYBOARD_RING_BUFFER_SIZE];
  inline static int head = 0;
  inline static int tail = 0;
  inline static bool special_extended = false;

public:
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
    if (Asm::inb(KEYBOARD_STATUS_PORT) & 1) {
      KeyEvent ev;
      KeyType ktype;
      unsigned char scancode = Asm::inb(KEYBOARD_PORT);

      if (scancode == SPECIAL_KEY_SCANCODE) {
        special_extended = true;
        return;
      }

      if (scancode & RELEASE_MASK) {
        special_extended = false;
        return;
      }

      char ascii = 0;
      if (special_extended) {
        if (scancode == DELETE_SCANCODE)
          ascii = '\b';
        if (scancode == ARROW_UP_SCANCODE) {
          ascii = KEY_ARROW_UP;
          ktype = KeyType::ArrowUp;
        }
        if (scancode == ARROW_DOWN_SCANCODE) {
          ascii = KEY_ARROW_DOWN;
          ktype = KeyType::ArrowDown;
        }
        if (scancode == ARROW_LEFT_SCANCODE) {
          ascii = KEY_ARROW_LEFT;
          ktype = KeyType::ArrowLeft;
        }
        if (scancode == ARROW_RIGHT_SCANCODE) {
          ascii = KEY_ARROW_RIGHT;
          ktype = KeyType::ArrowRight;
        }

        special_extended = false;
      } else {
        if (scancode == BACKSPACE_SCANCODE) {
          ascii = '\b';
          ktype = KeyType::BackSpace;
        } else if (scancode == ENTER_SCANCODE) {
          ascii = '\n';
          ktype = KeyType::Enter;
        } else {
          ascii = StringUtils::scancode_to_ascii(scancode);
          ktype = KeyType::Char;
        }
      }

      ev.scancode = ascii;
      ev.keytype = ktype;

      if (ascii != 0) {
        push(ev);
      }
    }
    return;
  }
};

extern "C" void c_keyboard_handler() { Keyboard::interrupt_handler(); }
