#pragma once

#include "../../boot/multiboot2.hpp"
#include "../core/asm.hpp"
#include "../core/timer.hpp"
#include "../random/random.hpp"
#include "../shared/types.hpp"
#include "../utils/math_utils.hpp"

enum class MouseButton { LEFT_BUTTON, RIGHT_BUTTON, MIDDLE_BUTTON, NONE };
enum class MouseEventType { MOVE, PRESS, RELEASE, NONE };

struct MouseEvent {
  MouseButton button_type;   // eg LEFT_CLICK / RIGHT_CLICK / MIDDLE_CLICK
  MouseEventType event_type; // eg MOVE / PRESS / RELEASE
};

class Mouse {

private:
  static constexpr u16 DATA_PORT = 0x60;
  static constexpr u16 COMMAND_PORT = 0x64;

  static constexpr u8 ENABLE_AUX_PORT = 0xA8;
  static constexpr u8 READ_CONFIG = 0x20;
  static constexpr u8 WRITE_CONFIG = 0x60;
  static constexpr u8 WRITE_TO_MOUSE = 0xD4;

  static constexpr u8 ENABLE_DATA_REPORTING = 0xF4;
  static constexpr u8 ACK = 0xFA;

  inline static u8 packet[3];
  inline static int packet_index = 0;

  inline static int x = 0;
  inline static int y = 0;

  inline static u8 mouse_flags = 0;
  inline static bool was_down = false;
  inline static u8 was_down_flags = 0;

  static constexpr auto MOUSE_RING_BUFFER_SIZE = 256;

  // Circular buffer implementation for header-only 
  inline static MouseEvent buffer[MOUSE_RING_BUFFER_SIZE];
  inline static int head = 0;
  inline static int tail = 0;

  static void set_flags(u8 new_flags) { mouse_flags = new_flags; }

  static bool wait_for_read() {
    for (int i = 0; i < 1000000; i++) {
      if (Asm::inb(COMMAND_PORT) & 0x01)
        return true;
    }

    return false;
  }

  static void wait_for_write() {
    while (Asm::inb(COMMAND_PORT) & 0x02)
      ;
  }

  static void write_controller(u8 command) {
    wait_for_write();
    Asm::outb(COMMAND_PORT, command);
  }

  static void write_mouse(u8 command) {
    wait_for_write();
    Asm::outb(DATA_PORT, command);
  }

  static bool read_data(u8 &data) {
    if (!wait_for_read())
      return false;

    data = Asm::inb(DATA_PORT);
    return true;
  }

  enum class Flags {
    LEFT_BUTTON = 0,
    RIGHT_BUTTON = 1,
    MIDDLE_BUTTON = 2,
    ALWAYS_1_FOR_SYNC = 3,
    X_SIGN = 4,
    Y_SIGN = 5,
    X_OVERFLOW = 6,
    Y_OVERFLOW = 7
  };

public:
  static void set_coords(int x_, int y_) {
    x = x_;
    y = y_;
  }
  static void init() {
    write_controller(ENABLE_AUX_PORT);
    write_controller(READ_CONFIG);

    u8 config;
    if (!read_data(config))
      return;

    // Keep IRQ12 disabled while we configure the mouse (0 = disabled).
    config &= ~(1 << 1);
    // Enable the auxiliary clock (0 = clock enabled).
    config &= ~(1 << 5);

    write_controller(WRITE_CONFIG);
    write_mouse(config);

    write_controller(WRITE_TO_MOUSE);
    write_mouse(ENABLE_DATA_REPORTING);

    u8 response;
    if (!read_data(response))
      return;
    if (response != ACK)
      return;

    // Now actually enable IRQ12 (1 = enabled).
    config |= (1 << 1);
    write_controller(WRITE_CONFIG);
    write_mouse(config);
  }

  static bool is_flag_active(u8 flags, Flags flag) {
    return flags & (1 << static_cast<u8>(flag));
  }

  static void interrupt_handler() {
    u8 data = Asm::inb(DATA_PORT);

    // byte 0 must always have bit 3 set. If we're out of
    // sync (eg missed a byte), drop bytes until we see one that
    // looks like a valid start-of-packet.
    if (packet_index == 0 &&
        !(data & (1 << static_cast<u8>(Flags::ALWAYS_1_FOR_SYNC))))
      return;

    packet[packet_index++] = data;

    if (packet_index < 3)
      return;

    packet_index = 0;

    u8 flags = packet[0];

    // Overflow means garbage data so we discard the packet.
    if (is_flag_active(flags, Flags::X_OVERFLOW) ||
        is_flag_active(flags, Flags::Y_OVERFLOW))
      return;

    set_flags(flags);

    int dx = packet[1];
    int dy = packet[2];

    if (is_flag_active(flags, Flags::X_SIGN))
      dx -= 256;
    if (is_flag_active(flags, Flags::Y_SIGN))
      dy -= 256;

    // PS/2 Y is inverted relative to screen coordinates (positive = up).
    int new_x = x + dx;
    int new_y = y - dy;

    new_x = MathUtils::clamp(new_x, 0, (int)Multiboot2::framebuffer.width - 1);
    new_y = MathUtils::clamp(new_y, 0, (int)Multiboot2::framebuffer.height - 1);

    bool moved = (new_x != x || new_y != y);

    set_coords(new_x, new_y);

    // Adding more entropy for better unpredictability of our Random::next()
    if (Timer::get_ticks() % (new_x + new_y + 1) == 0) {
      Random::add_entropy(Timer::ticks ^
                          ((new_x * new_y + 1) % (new_x + new_y + 1) + 1) / 10);
    }

    // only emit PRESS on the down-transition and RELEASE
    // on the up-transition, instead of re-emitting PRESS on every packet
    // while a button is held (which was flooding the ring buffer and
    // causing repeated route_mouse_event calls per physical click).
    bool now_down = is_any_button_down();

    MouseEventType type;
    MouseButton button;

    if (now_down && !was_down) {
      type = MouseEventType::PRESS;
      button = get_event_mouse_button(
          flags); // buttons are down now
    } else if (!now_down && was_down) {
      type = MouseEventType::RELEASE;
      button = get_event_mouse_button(
          was_down_flags); // use the flags from BEFORE release
    } else if (moved) {
      type = MouseEventType::MOVE;
      button = MouseButton::NONE;
    } else {
      type = MouseEventType::NONE;
      button = MouseButton::NONE;
    }

    if (now_down)
      was_down_flags = flags;

    was_down = now_down;

    if (type != MouseEventType::NONE)
      push(MouseEvent(button, type));
  }

  static int get_x() { return x; }

  static int get_y() { return y; }

  /* static u8 get_packet_byte(int index) {
    if (index < 0 || index >= 3)
      return 0;
  } */

  static constexpr bool is_any_button_down() {
    return is_button_down(MouseButton::LEFT_BUTTON) ||
           is_button_down(MouseButton::RIGHT_BUTTON) ||
           is_button_down(MouseButton::MIDDLE_BUTTON);
  }

  static constexpr bool is_button_down(MouseButton b) {
    switch (b) {
    case MouseButton::LEFT_BUTTON:
      return is_flag_active(mouse_flags, Flags::LEFT_BUTTON);
    case MouseButton::RIGHT_BUTTON:
      return is_flag_active(mouse_flags, Flags::RIGHT_BUTTON);
    case MouseButton::MIDDLE_BUTTON:
      return is_flag_active(mouse_flags, Flags::MIDDLE_BUTTON);
    case MouseButton::NONE:
      return false;
    }
    return false;
  }
  static constexpr bool is_left_button_down() {
    return is_button_down(MouseButton::LEFT_BUTTON);
  }
  static constexpr bool is_right_button_down() {
    return is_button_down(MouseButton::RIGHT_BUTTON);
  }
  static constexpr bool is_middle_button_down() {
    return is_button_down(MouseButton::MIDDLE_BUTTON);
  }

  static constexpr void push(MouseEvent ev) {
    int next_head = (head + 1) % MOUSE_RING_BUFFER_SIZE;
    if (next_head != tail) {
      buffer[head] = ev;
      head = next_head;
    }
  }

  static constexpr MouseButton get_event_mouse_button(u8 flags) {
    if (is_flag_active(flags, Flags::LEFT_BUTTON)) {
      return MouseButton::LEFT_BUTTON;
    }
    if (is_flag_active(flags, Flags::RIGHT_BUTTON)) {
      return MouseButton::RIGHT_BUTTON;
    }
    if (is_flag_active(flags, Flags::MIDDLE_BUTTON)) {
      return MouseButton::MIDDLE_BUTTON;
    }
    return MouseButton::NONE;
  }
  static constexpr MouseEvent read() {
    if (tail == head)
      return {MouseButton::NONE, MouseEventType::NONE};
    MouseEvent ev = buffer[tail];
    tail = (tail + 1) % MOUSE_RING_BUFFER_SIZE;
    return ev;
  }
};

extern "C" void c_mouse_handler() { Mouse::interrupt_handler(); }
