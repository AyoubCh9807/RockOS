#pragma once

#include "../shared/types.hpp"
#include "../utils/bit_utils.hpp"
#include "disk.hpp"

constexpr u32 BLOCK_SIZE = 512;
constexpr u32 TOTAL_BLOCKS = 20000;
// Filesystem layout
constexpr u32 BLOCK_BITMAP_START = 2;
constexpr u32 DATA_BLOCK_START = 135;
constexpr u32 BLOCK_BITMAP_SIZE = (TOTAL_BLOCKS + 7) / 8;
constexpr u32 BLOCK_BITMAP_SECTORS =
    (BLOCK_BITMAP_SIZE + BLOCK_SIZE - 1) / BLOCK_SIZE;
constexpr u32 BITS_PER_SECTOR = BLOCK_SIZE * 8;

// Invalid block return value
constexpr u32 INVALID_BLOCK = 0xFFFFFFFF;

class BlockManager {

private:
  Disk &disk;

  u32 block_bitmap_start;
  u32 data_block_start;

public:
  BlockManager(Disk &disk)
      : disk(disk), block_bitmap_start(BLOCK_BITMAP_START),
        data_block_start(DATA_BLOCK_START) {}
  u32 allocate_block() {
    u8 buffer[BLOCK_SIZE];

    u32 sectors_needed = (TOTAL_BLOCKS + BITS_PER_SECTOR - 1) / BITS_PER_SECTOR;

    for (u32 sector = 0; sector < sectors_needed; sector++) {

      u32 bits_to_search = BITS_PER_SECTOR;

      if (sector == sectors_needed - 1)
        bits_to_search = TOTAL_BLOCKS % BITS_PER_SECTOR;

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
    /*

    Step 1:
    Convert filesystem block number into actual disk LBA.

    LBA = data_block_start + block_number


    Step 2:
    Read the sector from disk into buffer.


    Step 3:
    Return success/failure.

    */
  }

  bool write_block(u32 block_number, const u8 *buffer) {
    /*

    Step 1:
    Convert filesystem block number into actual disk LBA.


    Step 2:
    Write buffer contents to disk sector.


    Step 3:
    Return success/failure.

    */
  }
};
