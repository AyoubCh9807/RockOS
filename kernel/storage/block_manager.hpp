#pragma once

#include "../shared/types.hpp"
#include "../utils/bit_utils.hpp"
#include "disk.hpp"
class BlockManager {

private:
  Disk &disk;

  static constexpr u32 block_bitmap_start = BLOCK_BITMAP_START;
  static constexpr u32 data_block_start = DATA_BLOCK_START;

public:
  BlockManager(Disk &disk) : disk(disk) {};

  bool format() {
    u8 buffer[BLOCK_SIZE] = {};

    for (u32 i = 0; i < BLOCK_BITMAP_SECTORS; i++) {
      if (!disk.write_sector(BLOCK_BITMAP_START + i, buffer))
        return false;
    }

    return true;
  }

  u32 allocate_block() {
    u8 buffer[BLOCK_SIZE];

    for (u32 sector = 0; sector < BLOCK_BITMAP_SECTORS; sector++) {

      u32 bits_to_search =
          (sector == BLOCK_BITMAP_SECTORS - 1 && TOTAL_BLOCKS % BITS_PER_SECTOR)
              ? TOTAL_BLOCKS % BITS_PER_SECTOR
              : BITS_PER_SECTOR;

      disk.read_sector(block_bitmap_start + sector, buffer);

      u32 free_bit = BitUtils::find_first_free_bit(buffer, bits_to_search);

      if (free_bit != INVALID_BLOCK) {
        BitUtils::set_bit(buffer, free_bit);

        disk.write_sector(block_bitmap_start + sector, buffer);

        return sector * BITS_PER_SECTOR + free_bit;
      }
    }

    return INVALID_BLOCK;
  }

  bool free_block(u32 block_number) {

    if (block_number >= TOTAL_BLOCKS)
      return false;

    u32 bitmap_sector = block_number / BITS_PER_SECTOR;
    u8 buffer[BLOCK_SIZE];
    disk.read_sector(block_bitmap_start + bitmap_sector, buffer);

    u32 bit_index = block_number % BITS_PER_SECTOR;
    if (!BitUtils::is_bit_set(buffer, bit_index))
      return false;

    BitUtils::clear_bit(buffer, bit_index);

    disk.write_sector(block_bitmap_start + bitmap_sector, buffer);
    return true;
  }

  bool read_block(u32 block_number, u8 *buffer) {

    if (block_number >= TOTAL_BLOCKS)
      return false;

    u32 lba = data_block_start + block_number;
    disk.read_sector(lba, buffer);
    return true;
  }

  bool write_block(u32 block_number, const u8 *buffer) {

    if (block_number >= TOTAL_BLOCKS)
      return false;

    u32 lba = data_block_start + block_number;
    disk.write_sector(lba, buffer);

    return true;
  }
};
