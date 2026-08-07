#pragma once

#include "../shared/types.hpp"
#include "../utils/bit_utils.hpp"

#include "disk.hpp"
#include "layout.hpp"

class InodeManager {
private:
  Disk &disk;
  u32 inode_table_start;
  u32 inode_bitmap_start;

public:
  InodeManager(Disk &disk)
      : disk(disk), inode_bitmap_start(INODE_BITMAP_START),
        inode_table_start(INODE_TABLE_START) {};

  void format() {
    u8 buffer[BLOCK_SIZE];

    for (int i = 0; i < BLOCK_SIZE; i++)
      buffer[i] = 0;

    // clear inode bitmap
    for (u32 i = 0; i < INODE_BITMAP_SECTORS; i++) {
      disk.write_sector(INODE_BITMAP_START + i, buffer);
    }

    // clear inode table
    for (u32 i = 0; i < INODES_PER_SECTOR; i++) {
      disk.write_sector(INODE_TABLE_START + i, buffer);
    }
  }

  u32 allocate_inode() {

    u8 inode_bitmap[BLOCK_SIZE];
    disk.read_sector(inode_bitmap_start, inode_bitmap);

    // find free inode number

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
  bool free_inode(u32 inode_num) {
    if (inode_num >= TOTAL_INODES)
      return false;

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
      return true;
    }
    return false;
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
    if (inode_number >= TOTAL_INODES)
      return false;
    u32 inode_sector = inode_table_start + (inode_number / INODES_PER_SECTOR);
    alignas(Inode) u8 buffer[BLOCK_SIZE];
    disk.read_sector(inode_sector, buffer);

    Inode *inodes = reinterpret_cast<Inode *>(buffer);
    u32 inode_offset = inode_number % INODES_PER_SECTOR;

    inodes[inode_offset] = inode;

    disk.write_sector(inode_sector, buffer);
    return true;
  }

  bool mark_used(u32 inode_number) {
    Inode inode;
    bool is_read = read_inode(inode_number, inode);
    if (!is_read)
      return false;
    inode.used = true;
    write_inode(inode_number, inode);
    return true;
  }

  bool reserve_inode(u32 inode_number) {
    // set_bitmap_bit(inode_number);

    u8 inode_bitmap[BLOCK_SIZE];
    bool is_read = disk.read_sector(inode_bitmap_start, inode_bitmap);
    if (!is_read)
      return false;

    BitUtils::set_bit(inode_bitmap, inode_number);
    return disk.write_sector(inode_bitmap_start, inode_bitmap);
  }

};
