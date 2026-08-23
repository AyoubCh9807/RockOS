#pragma once
#include "../shared/types.hpp"

namespace Math {
static constexpr u8 bcd_to_binary(u8 value) {
  return (value & 0x0F) + ((value >> 4) * 10);
}
} // namespace Math
