#pragma once

#include "parser_types.hpp"

class RegisterManager {
private:
  // Bit i = 1 -> register is used.
  // Bit i = 0 -> register is free.
  u32 used_mask = 0;

  // Allocation preference.
  static constexpr Register REGISTER_ORDER[] = {
      Register::EAX,
      Register::EBX,
      Register::ECX,
      Register::EDX,
      Register::ESI,
      Register::EDI};

  static constexpr u32 REGISTER_COUNT = 6;

  u32 register_bit(Register reg) const {
    switch (reg) {
    case Register::EAX:
      return 1u << 0;

    case Register::EBX:
      return 1u << 1;

    case Register::ECX:
      return 1u << 2;

    case Register::EDX:
      return 1u << 3;

    case Register::ESI:
      return 1u << 4;

    case Register::EDI:
      return 1u << 5;

    default:
      return 0;
    }
  }

public:
  Register alloc() {
    for (u32 i = 0; i < REGISTER_COUNT; i++) {
      Register reg = REGISTER_ORDER[i];

      if (is_free(reg)) {
        used_mask |= register_bit(reg);
        return reg;
      }
    }

    return Register::INVALID;
  }

  void free(Register reg) {
    u32 bit = register_bit(reg);

    if (bit == 0)
      return;

    used_mask &= ~bit;
  }

  bool is_used(Register reg) const {
    u32 bit = register_bit(reg);

    if (bit == 0)
      return false;

    return (used_mask & bit) != 0;
  }

  bool is_free(Register reg) const {
    return !is_used(reg);
  }

  bool full() const {
    return used_mask == ((1u << REGISTER_COUNT) - 1);
  }

  void clear() {
    used_mask = 0;
  }

  u32 used_count() const {
    u32 count = 0;
    u32 mask = used_mask;

    while (mask) {
      count += mask & 1;
      mask >>= 1;
    }

    return count;
  }

  u32 free_count() const {
    return REGISTER_COUNT - used_count();
  }
};
