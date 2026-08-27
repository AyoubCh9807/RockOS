#pragma once
#include "../shared/types.hpp"

namespace MathUtils{
static constexpr u8 bcd_to_binary(u8 value) {
  return (value & 0x0F) + ((value >> 4) * 10);
}
template <typename T> T expr_result(T x, T y, char op) {
  switch (op) {
  case '+':
    return x + y;

  case '-':
    return x - y;

  case '*':
    return x * y;

  case '/':
    return x / y;

  default:
    return 0;
  }
}

static void int_to_hex(u32 value, char *buf) {
  const char *hex = "0123456789ABCDEF";

  buf[0] = '0';
  buf[1] = 'x';

  for (int i = 0; i < 8; i++) {
    int shift = (7 - i) * 4;
    buf[2 + i] = hex[(value >> shift) & 0xF];
  }

  buf[10] = '\0';
}

} // namespace Math
