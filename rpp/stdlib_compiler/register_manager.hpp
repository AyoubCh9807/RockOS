#pragma once

#include "parser_types.hpp"
#include "../../kernel/shared/types.hpp"

class RegisterManager {
private:
  // Bit i = 1 -> register is used.
  // Bit i = 0 -> register is free.

  u32 used_mask = 0;
  u32 used_float_mask = 0;

  // Integer register allocation preference.
  static constexpr Register REGISTER_ORDER[] = {
      Register::EAX,
      Register::EBX,
      Register::ECX,
      Register::EDX,
      Register::ESI,
      Register::EDI};

  // Floating-point register allocation preference.
  static constexpr Register FLOAT_REGISTER_ORDER[] = {
      Register::XMM0,
      Register::XMM1,
      Register::XMM2,
      Register::XMM3,
      Register::XMM4,
      Register::XMM5,
      Register::XMM6,
      Register::XMM7};

  static constexpr u32 REGISTER_COUNT = 6;
  static constexpr u32 FLOAT_REGISTER_COUNT = 8;

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

  u32 float_register_bit(Register reg) const {
    switch (reg) {
    case Register::XMM0:
      return 1u << 0;

    case Register::XMM1:
      return 1u << 1;

    case Register::XMM2:
      return 1u << 2;

    case Register::XMM3:
      return 1u << 3;

    case Register::XMM4:
      return 1u << 4;

    case Register::XMM5:
      return 1u << 5;

    case Register::XMM6:
      return 1u << 6;

    case Register::XMM7:
      return 1u << 7;

    default:
      return 0;
    }
  }

public:

  // Integer registers

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

  // Floating-point registers

  Register alloc_float() {
    for (u32 i = 0; i < FLOAT_REGISTER_COUNT; i++) {
      Register reg = FLOAT_REGISTER_ORDER[i];

      if (is_float_free(reg)) {
        used_float_mask |= float_register_bit(reg);
        return reg;
      }
    }

    return Register::INVALID;
  }

  // Free

  void free(Register reg) {
    u32 bit = register_bit(reg);

    if (bit != 0) {
      used_mask &= ~bit;
      return;
    }

    u32 float_bit = float_register_bit(reg);

    if (float_bit != 0)
      used_float_mask &= ~float_bit;
  }

  // Integer queries

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

  // Floating-point queries

  bool is_float_used(Register reg) const {
    u32 bit = float_register_bit(reg);

    if (bit == 0)
      return false;

    return (used_float_mask & bit) != 0;
  }

  bool is_float_free(Register reg) const {
    return !is_float_used(reg);
  }

  bool float_full() const {
    return used_float_mask ==
           ((1u << FLOAT_REGISTER_COUNT) - 1);
  }

  u32 float_used_count() const {
    u32 count = 0;
    u32 mask = used_float_mask;

    while (mask) {
      count += mask & 1;
      mask >>= 1;
    }

    return count;
  }

  u32 float_free_count() const {
    return FLOAT_REGISTER_COUNT - float_used_count();
  }

  void clear() {
    used_mask = 0;
    used_float_mask = 0;
  }
};
