#pragma once

#include "../../boot/multiboot2.hpp"
#include "../core/asm.hpp"
#include "../shared/types.hpp"
#include "../utils/math_utils.hpp"

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

  // Debug state is only written by the interrupt handler.
  // The desktop reads it later while rendering.
  inline static volatile u32 irq_count = 0;
  inline static volatile u8 last_byte = 0;
  inline static volatile u8 debug_packet[3] = {0, 0, 0};

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

  static void set_coords(int x_, int y_) {
    x = x_;
    y = y_;
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
    irq_count++;
    last_byte = data;

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

    debug_packet[0] = packet[0];
    debug_packet[1] = packet[1];
    debug_packet[2] = packet[2];

    u8 flags = packet[0];

    // Overflow means garbage data so we discard the packet.
    if (is_flag_active(flags, Flags::X_OVERFLOW) ||
        is_flag_active(flags, Flags::Y_OVERFLOW))
      return;

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

    set_coords(new_x, new_y);
  }

  static int get_x() { return x; }

  static int get_y() { return y; }

  static u32 get_irq_count() { return irq_count; }

  static u8 get_last_byte() { return last_byte; }

  static u8 get_packet_byte(int index) {
    if (index < 0 || index >= 3)
      return 0;

    return debug_packet[index];
  }
};

extern "C" void c_mouse_handler() { Mouse::interrupt_handler(); }
