#pragma once
#include "../shared/types.hpp"

struct SuperBlock {
  u32 magic;
  u32 total_blocks;
  u32 total_inodes;
  u32 inode_bitmap_start;
  u32 block_bitmap_start;
  u32 inode_table_start;
  u32 data_block_start;
  size_t size;
};
constexpr u32 SECTOR_SIZE = 512;
constexpr u32 MAX_FILENAME_LENGTH = 32;

struct DirectoryEntry {
  char name[MAX_FILENAME_LENGTH];
  u32 inode_number;
  bool is_used;
};

struct ParentPath {
  u32 parent_inode;
  char name[MAX_FILENAME_LENGTH];
};

constexpr int MAX_FILE_ENTRY_NAME_LENGTH = 32;

struct FileEntry {

  char name[MAX_FILE_ENTRY_NAME_LENGTH];
  u32 size;
  u32 start_block;
  bool is_dir;
  bool is_used;
};

constexpr u32 BLOCK_SIZE = 512;

constexpr int DIRECT_BLOCKS = 12;

constexpr u32 FS_MAGIC = 0x524F434B; // ROCK

// Structs redefined for size calculations

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
constexpr u32 INODE_SIZE = sizeof(Inode);

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
constexpr u32 INODE_TABLE_SECTORS =
    (TOTAL_INODES * sizeof(Inode) + BLOCK_SIZE - 1) / BLOCK_SIZE;

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

constexpr u32 DIRECTORY_ENTRIES_PER_BLOCK = BLOCK_SIZE / sizeof(DirectoryEntry);

constexpr u32 DATA_BLOCK_START = INODE_TABLE_START + INODE_TABLE_SECTORS;

// List buffer size
constexpr int LIST_BUFFER_SIZE =
    DIRECT_BLOCKS * DIRECTORY_ENTRIES_PER_BLOCK * (MAX_FILENAME_LENGTH + 1);
