#pragma once

#include "../shared/types.hpp"

class BitUtils {
public:
	static constexpr u32 BITS_PER_BYTE = 8;
	static constexpr u32 INVALID_BIT_INDEX = 0xFFFFFFFF;

	// Returns true if a specific bit is set (1)
	static bool is_bit_set(const u8 *bitmap, u32 bit_index) {
		if (!bitmap) return false;
		u32 byte = bit_index / BITS_PER_BYTE;
		u32 bit  = bit_index % BITS_PER_BYTE;
		return (bitmap[byte] & (u8)(1u << bit)) != 0;
	}

	// Sets a specific bit to 1
	static void set_bit(u8 *bitmap, u32 bit_index) {
		if (!bitmap) return;
		u32 byte = bit_index / BITS_PER_BYTE;
		u32 bit  = bit_index % BITS_PER_BYTE;
		bitmap[byte] |= (u8)(1u << bit);
	}

	// Clears a specific bit to 0
	static void clear_bit(u8 *bitmap, u32 bit_index) {
		if (!bitmap) return;
		u32 byte = bit_index / BITS_PER_BYTE;
		u32 bit  = bit_index % BITS_PER_BYTE;
		bitmap[byte] &= (u8)~(1u << bit);
	}

	// Finds the first bit that is 0
	// Returns the bit index, or INVALID_BIT_INDEX if no free bit exists
	static u32 find_first_free_bit(const u8 *bitmap, u32 total_bits) {
		if (!bitmap) return INVALID_BIT_INDEX;

		for (u32 i = 0; i < total_bits; i++) {
			if (!is_bit_set(bitmap, i)) return i;
		}
		return INVALID_BIT_INDEX;
	}

	static u32 get_total_bits(u32 bitmap_size_bytes) {
		return bitmap_size_bytes * BITS_PER_BYTE;
	}
};
