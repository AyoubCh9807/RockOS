#pragma once

#include "../shared/types.hpp"
#include "disk.hpp"
#include "inode.hpp"
constexpr u32 BLOCK_SIZE = 512;

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
constexpr u32 DATA_BLOCK_START = 135;

constexpr u32 INVALID_INODE = 0xFFFFFFFF;
class InodeManager {
private:
  Disk &disk;
  u32 inode_table_start;
  u32 inode_bitmap_start;

public:
  InodeManager(Disk &disk)
      : disk(disk), inode_bitmap_start(INODE_BITMAP_START),
        inode_table_start(INODE_TABLE_START) {};

  u32 allocate_inode() {

    u8 inode_bitmap[BLOCK_SIZE];
    disk.read_sector(inode_bitmap_start, inode_bitmap);

    // find free inode number IDK HOW

    for (u32 inode_num = 0; inode_num < TOTAL_INODES; inode_num++) {
      u32 byte = inode_num / BITS_PER_BYTE;
      u32 bit = inode_num % BITS_PER_BYTE;
      if ((inode_bitmap[byte] & (1 << bit)) == 0) {
        // This inode is free

        // We mark is as non free
        inode_bitmap[byte] |= (1 << bit);

        // Update inode bitmap
        disk.write_sector(inode_bitmap_start, inode_bitmap);

        // Initialize a new inode
        Inode inode{};
        inode.is_directory = false;
        inode.used = true;
        inode.id = inode_num;

        // write the inode
        write_inode(inode.id, inode);

        // return the inode number
        return inode.id;
      }
    }
    return INVALID_INODE;
  }
  void free_inode(u32 inode_num) {
    u8 inode_bitmap[BLOCK_SIZE];
    disk.read_sector(inode_bitmap_start, inode_bitmap);

    // find the current node

    u32 byte = inode_num / 8;
    u32 bit = inode_num % 8;
    if ((inode_bitmap[byte] & (1 << bit)) != 0) {
      // This inode is used
      // We mark is as free free
      inode_bitmap[byte] &= ~(1 << bit);

      // Update inode bitmap
      disk.write_sector(inode_bitmap_start, inode_bitmap);
      Inode inode{};

      inode.id = inode_num;
      inode.used = false;

      write_inode(inode_num, inode);
    }
  }
  bool read_inode(u32 inode_number, Inode &inode) {

    if (inode_number >= TOTAL_INODES)
      return false;

    u32 inode_sector = inode_table_start + (inode_number / INODES_PER_SECTOR);
    alignas(Inode) u8 temp_buffer[BLOCK_SIZE];

    disk.read_sector(inode_sector, temp_buffer);

    u32 inode_offset = inode_number % INODES_PER_SECTOR;

    Inode *inodes = reinterpret_cast<Inode *>(temp_buffer);
    inode = inodes[inode_offset];
    return true;
  }
  bool write_inode(u32 inode_number, const Inode &inode) {
    u32 inode_sector = inode_table_start + (inode_number / INODES_PER_SECTOR);
    alignas(Inode) u8 buffer[BLOCK_SIZE];
    disk.read_sector(inode_sector, buffer);

    Inode *inodes = reinterpret_cast<Inode *>(buffer);
    u32 inode_offset = inode_number % INODES_PER_SECTOR;

    inodes[inode_offset] = inode;

    disk.write_sector(inode_sector, buffer);
    return true;
  }
};
