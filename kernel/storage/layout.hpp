#pragma once
#include "../shared/types.hpp"
// enable/disable debugging
constexpr int FS_DEBUG = 0;

struct SuperBlock {
  u32 magic;
  u32 total_blocks;
  u32 total_inodes;
  u32 inode_bitmap_start;
  u32 block_bitmap_start;
  u32 inode_table_start;
  u32 data_block_start;
  u32 size;
};
constexpr u32 SECTOR_SIZE = 512;
constexpr u32 MAX_FILENAME_LENGTH = 32;
struct DirectoryEntry {
  char name[MAX_FILENAME_LENGTH];
  u32 inode_number;
  bool is_used;
};
constexpr u32 BLOCK_SIZE = 512;
constexpr int DIRECT_BLOCKS = 12;
constexpr u32 FS_MAGIC = 0x524F434B; // ROCK
struct Inode {
  u32 id;
  u32 size;
  bool is_directory;
  u32 direct_blocks[DIRECT_BLOCKS];
  u32 indirect_block;
  u32 parent_inode;
  bool used;
};
constexpr u32 TOTAL_INODES = 1024;
constexpr u32 TOTAL_BLOCKS = 20000;
constexpr u32 INVALID_INODE = 0xFFFFFFFF;
constexpr u32 INVALID_BLOCK = 0xFFFFFFFF;
constexpr u32 ROOT_INODE = 0;
// ----- Derived sizes -----
constexpr u32 BITS_PER_BYTE = 8;
constexpr u32 BITS_PER_SECTOR = BLOCK_SIZE * BITS_PER_BYTE;
// inode bitmap
constexpr u32 INODE_BITMAP_SIZE =
    (TOTAL_INODES + BITS_PER_BYTE - 1) / BITS_PER_BYTE;
constexpr u32 INODE_BITMAP_SECTORS =
    (INODE_BITMAP_SIZE + BLOCK_SIZE - 1) / BLOCK_SIZE;
// block bitmap
constexpr u32 BLOCK_BITMAP_SIZE =
    (TOTAL_BLOCKS + BITS_PER_BYTE - 1) / BITS_PER_BYTE;
constexpr u32 BLOCK_BITMAP_SECTORS =
    (BLOCK_BITMAP_SIZE + BLOCK_SIZE - 1) / BLOCK_SIZE;
// inode table sectors
constexpr u32 INODE_SIZE = sizeof(Inode);
static_assert(INODE_SIZE > 0, "Inode size must be > 0");
constexpr u32 INODES_PER_SECTOR = BLOCK_SIZE / INODE_SIZE;
static_assert(INODES_PER_SECTOR > 0,
              "Inode is too big to fit in a 512-byte sector");
// *** BUG FIX: this used to be (TOTAL_INODES * INODE_SIZE + BLOCK_SIZE - 1) /
// BLOCK_SIZE, which assumes inodes pack tightly across sector boundaries with
// no waste. They don't, InodeManager addresses inodes as (inode_number /
// INODES_PER_SECTOR), and INODES_PER_SECTOR floors to 7 here (512/72), wasting
// 56 bytes at the end of every sector instead of spilling into the next one.
// The old formula therefore reserved too few sectors (144 instead of the 147
// actually needed for 1024 inodes at 7-per-sector), so high inode numbers
// silently wrote into what the block allocator thought was free data space.
// Computing it from INODES_PER_SECTOR instead matches the real addressing
// scheme. ***
constexpr u32 INODE_TABLE_SECTORS =
    (TOTAL_INODES + INODES_PER_SECTOR - 1) / INODES_PER_SECTOR;
// Disk layout (NO hardcoded 7 anymore) 
constexpr u32 SUPERBLOCK_START = 1;
constexpr u32 INODE_BITMAP_START = SUPERBLOCK_START + 1;
constexpr u32 BLOCK_BITMAP_START = INODE_BITMAP_START + INODE_BITMAP_SECTORS;
constexpr u32 INODE_TABLE_START = BLOCK_BITMAP_START + BLOCK_BITMAP_SECTORS;
constexpr u32 DATA_BLOCK_START = INODE_TABLE_START + INODE_TABLE_SECTORS;
// directory capacity
constexpr u32 DIRECTORY_ENTRIES_PER_BLOCK = BLOCK_SIZE / sizeof(DirectoryEntry);
// List buffer size (rough)
constexpr int LIST_BUFFER_SIZE =
    DIRECT_BLOCKS * DIRECTORY_ENTRIES_PER_BLOCK * (MAX_FILENAME_LENGTH + 1);
