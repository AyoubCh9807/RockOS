#pragma once

#include "../shared/types.hpp"

constexpr u32 FS_MAGIC = 0x524F434B; // ROCK
constexpr u32 SECTOR_SIZE = 512;
constexpr u32 MAX_FILENAME_LENGTH = 32;

struct Superblock {
  u32 magic;
  u32 total_blocks;
  u32 total_inodes;
  u32 inode_bitmap_start;
  u32 block_bitmap_start;
  u32 inode_table_start;
  u32 data_block_start;
};

struct DirectoryEntry {
  char name[MAX_FILENAME_LENGTH];
  u32 inode_number;
  bool is_used;
};
