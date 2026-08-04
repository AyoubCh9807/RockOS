#pragma once

#include "../shared/types.hpp"

class BitUtils {
public:
  static constexpr u32 BITS_PER_BYTE = 8;

  // Returns true if a specific bit is set (1)
  static bool is_bit_set(const u8 *bitmap, u32 bit_index) {
    u32 byte = bit_index / BITS_PER_BYTE;
    u32 bit = bit_index % BITS_PER_BYTE;

    return (bitmap[byte] & (1 << bit)) != 0;
  }

  // Sets a specific bit to 1
  static void set_bit(u8 *bitmap, u32 bit_index) {
    u32 byte = bit_index / BITS_PER_BYTE;
    u32 bit = bit_index % BITS_PER_BYTE;

    bitmap[byte] |= (1 << bit);
  }

  // Clears a specific bit to 0
  static void clear_bit(u8 *bitmap, u32 bit_index) {
    u32 byte = bit_index / BITS_PER_BYTE;
    u32 bit = bit_index % BITS_PER_BYTE;

    bitmap[byte] &= ~(1 << bit);
  }

  // Finds the first bit that is 0
  // Returns the bit index
  // Returns 0xFFFFFFFF if no free bit exists
  static u32 find_first_free_bit(const u8 *bitmap, u32 total_bits) {
    for (u32 i = 0; i < total_bits; i++) {
      if (!is_bit_set(bitmap, i))
        return i;
    }

    return 0xFFFFFFFF;
  }

  static u32 get_total_bits(u32 bitmap_size_bytes) {
    return bitmap_size_bytes * BITS_PER_BYTE;
  }
};
