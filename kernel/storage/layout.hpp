#pragma once
#include "../shared/types.hpp"
constexpr u32 BLOCK_SIZE = 512;

constexpr int DIRECT_BLOCKS = 12;

constexpr u32 FS_MAGIC = 0x524F434B; // ROCK
constexpr u32 SECTOR_SIZE = 512;
constexpr u32 MAX_FILENAME_LENGTH = 32;

constexpr u32 TOTAL_INODES = 1024;
constexpr u32 TOTAL_BLOCKS = 20000;
constexpr u32 INODE_SIZE = sizeof(u32) * 5 + sizeof(bool) * 2 + 1;

constexpr u32 INODES_PER_SECTOR = BLOCK_SIZE / INODE_SIZE;

// Bitmap constants
constexpr u32 BITS_PER_BYTE = 8;
constexpr u32 INODE_BITMAP_SIZE =
    (TOTAL_INODES + BITS_PER_BYTE - 1) / BITS_PER_BYTE;
constexpr u32 INODE_BITMAP_SECTORS =
    (INODE_BITMAP_SIZE + BLOCK_SIZE - 1) / BLOCK_SIZE;

// Layout
constexpr u32 SUPERBLOCK_START = 0;
constexpr u32 INODE_BITMAP_START = 1;
constexpr u32 BLOCK_BITMAP_START = INODE_BITMAP_START + INODE_BITMAP_SECTORS;
constexpr u32 INODE_TABLE_START = 7;
constexpr u32 DATA_BLOCK_START = 135;

constexpr u32 INVALID_INODE = 0xFFFFFFFF;
// Filesystem layout
constexpr u32 BLOCK_BITMAP_SIZE = (TOTAL_BLOCKS + 7) / 8;
constexpr u32 BLOCK_BITMAP_SECTORS =
    (BLOCK_BITMAP_SIZE + BLOCK_SIZE - 1) / BLOCK_SIZE;
constexpr u32 BITS_PER_SECTOR = BLOCK_SIZE * 8;

// Invalid block return value
constexpr u32 INVALID_BLOCK = 0xFFFFFFFF;

// Root node
constexpr u32 ROOT_INODE = 0;
