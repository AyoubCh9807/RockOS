#pragma once
#include "../shared/types.hpp"

constexpr int confirmation_port = 0x64;
constexpr int char_port = 0x60;
constexpr int release_hex = 0x80;
constexpr int special_key_hex = 0xE0;
constexpr int delete_hex = 0x53;
constexpr int backspace_hex = 0x0E;
constexpr int enter_hex = 0x1C;
constexpr int command_register_hex = 0x43;
constexpr int channel_zero_data_port = 0x40;
constexpr int keyboard_command_port = 0x64;
constexpr int reboot_data = 0xFE;

namespace Asm {

inline void outb(unsigned short port, unsigned char val) {
  __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

inline unsigned char inb(unsigned short port) {
  unsigned char result;
  __asm__ volatile("inb %1, %0" : "=a"(result) : "Nd"(port));
  return result;
}

static u16 inw(u16 port) {
  u8 low = inb(port);
  u8 high = inb(port);
  return (static_cast<u16>(high) << 8) | low;
}

static void outw(u16 port, u16 value) {
  // Send the low byte first, then the high byte
  outb(port, (u8)(value & 0xFF));
  outb(port, (u8)(value >> 8));
}
inline void halt() { __asm__("hlt"); }

} // namespace Asm
