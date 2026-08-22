#pragma once
#include "ps_types.hpp"

class FrameAllocator {
private:
  u8 *bitmap;
  u32 bitmap_size_bytes;
  u32 total_frames;

public:
  FrameAllocator(u32 total_memory) : bitmap(nullptr) {

    total_frames = total_memory / PAGE_SIZE;
    bitmap_size_bytes = (total_frames + 7) / 8;

    bitmap = new u8[bitmap_size_bytes];

    // Initially every frame is free.
    for (u32 i = 0; i < bitmap_size_bytes; i++)
      bitmap[i] = 0;

    BitUtils::set_bit(bitmap, 0);
  }

  FrameAllocatorEvent alloc() {

    u32 bit = BitUtils::find_first_free_bit(bitmap, total_frames);

    if (bit == BitUtils::INVALID_BIT_INDEX) {
      return FrameAllocatorEvent(0, false, FAllocatorEventType::ALLOCATED);
    }

    BitUtils::set_bit(bitmap, bit);

    u64 physical_address = (u64)bit * PAGE_SIZE;

    return FrameAllocatorEvent(physical_address, true,
                               FAllocatorEventType::ALLOCATED);
  }

  FrameAllocatorEvent free(u64 physical_address) {

    if (physical_address == 0 || physical_address % PAGE_SIZE != 0) {

      return FrameAllocatorEvent(physical_address, false,
                                 FAllocatorEventType::FREE);
    }

    u32 frame = physical_address / PAGE_SIZE;

    if (frame >= total_frames) {
      return FrameAllocatorEvent(physical_address, false,
                                 FAllocatorEventType::FREE);
    }

    if (BitUtils::is_bit_set(bitmap, frame))
      BitUtils::clear_bit(bitmap, frame);

    return FrameAllocatorEvent(physical_address, true,
                               FAllocatorEventType::FREE);
  }
};
