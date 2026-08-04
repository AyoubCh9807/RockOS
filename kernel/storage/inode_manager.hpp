#pragma once

#include "../shared/types.hpp"
#include "disk.hpp"
#include "inode.hpp"

constexpr u32 TOTAL_INODES = 1024;
constexpr u32 TOTAL_BLOCKS = 20000;
constexpr u32 BLOCK_SIZE = 512;

constexpr int SUPERBLOCK_START = 0;
constexpr int INODE_BITMAP_START = 1;
constexpr int BLOCK_BITMAP_START = 2;
constexpr int INODE_TABLE_START = 7;
constexpr int DATA_BLOCK_START = 135;

class InodeManager {
private:
  Disk &disk;
  u32 inode_table_start;
  u32 inode_bitmap_start;

public:
  InodeManager(Disk &disk) : disk(disk) {};

  u32 allocate_inode() {
    /*
    Read the inode bitmap.
    Find the first 0 bit (free inode).
    Set it to 1 (used).
    Write the bitmap back to disk.
    Create a fresh Inode with default values.
    Write that inode into the inode table.
    Return its inode number.
     * */
    u* buffer[BLOCK_SIZE];

    u32 *inode_table;
    disk.read_sector(inode_table_start, (u8 *)inode_table);


  }
  void free_inode(u32 inode) {}
  void read_inode(u32 inode_number, Inode &inode) {}
  bool write_inode(u32 inode_number, const Inode &inode) {}
};
