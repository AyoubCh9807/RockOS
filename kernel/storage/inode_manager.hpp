#pragma once

#include "../core/kernel.hpp"
#include "../utils/debugger.hpp"
#include "../utils/terminal_utils.hpp"
#include "disk.hpp"
#include "layout.hpp"

class InodeManager {

private:
  Disk &disk;

public:
  InodeManager(Disk &disk) : disk(disk) {}

  bool format() {
    // Clear inode bitmap
    u8 bitmap[BLOCK_SIZE] = {};

    for (u32 i = 0; i < INODE_BITMAP_SECTORS; i++) {
      if (!disk.write_sector(INODE_BITMAP_START + i, bitmap)) {
        Debugger::log("INODE BITMAP FORMAT WRITE FAILED AT SECTOR ");
        Debugger::log_number(INODE_BITMAP_START + i);
        Debugger::log("\n");
        return false;
      }
    }

    // Clear inode table
    u8 empty[BLOCK_SIZE] = {};

    for (u32 i = 0; i < INODE_TABLE_SECTORS; i++) {
      if (!disk.write_sector(INODE_TABLE_START + i, empty)) {
        Debugger::log("INODE TABLE FORMAT WRITE FAILED AT SECTOR ");
        Debugger::log_number(INODE_TABLE_START + i);
        Debugger::log("\n");
        return false;
      }
    }

    return true;
  }
  bool reserve_inode(u32 inode_number) {
    if (inode_number >= TOTAL_INODES)
      return false;

    u32 sector = INODE_BITMAP_START + (inode_number / BITS_PER_SECTOR);

    u32 bit = inode_number % BITS_PER_SECTOR;

    u8 buffer[BLOCK_SIZE];

    if (!disk.read_sector(sector, buffer))
      return false;

    u32 byte_index = bit / 8;
    u32 bit_index = bit % 8;

    buffer[byte_index] |= (1 << bit_index);

    return disk.write_sector(sector, buffer);
  }

  bool free_inode(u32 inode_number) {
    if (inode_number >= TOTAL_INODES)
      return false;

    u32 sector = INODE_BITMAP_START + (inode_number / BITS_PER_SECTOR);

    u32 bit = inode_number % BITS_PER_SECTOR;

    u8 buffer[BLOCK_SIZE];

    if (!disk.read_sector(sector, buffer))
      return false;

    u32 byte_index = bit / 8;
    u32 bit_index = bit % 8;

    buffer[byte_index] &= ~(1 << bit_index);

    if (!disk.write_sector(sector, buffer))
      return false;

    Inode empty{};

    empty.id = inode_number;
    empty.used = false;
    empty.is_directory = false;
    empty.size = 0;
    empty.parent_inode = 0;

    // IMPORTANT: Inode{} zero-initializes direct_blocks[]/indirect_block to 0,
    // but 0 is a VALID block number, not "unused" — INVALID_BLOCK
    // (0xFFFFFFFF) is the actual sentinel. Without this, a freed inode's
    // direct_blocks[0] reads back as block 0, which just happens to be the
    // very first block ever allocated (usually the root directory's own
    // data block). Any later code that reads this freed inode (e.g. an
    // orphaned directory entry still pointing at it) will misinterpret
    // block 0 as real data and can never treat it as empty/unallocated.
    for (int i = 0; i < DIRECT_BLOCKS; i++)
      empty.direct_blocks[i] = INVALID_BLOCK;
    empty.indirect_block = INVALID_BLOCK;

    return write_inode(inode_number, empty);
  }

  u32 allocate_inode() {
    for (u32 i = 0; i < TOTAL_INODES; i++) {
      u32 sector = INODE_BITMAP_START + (i / BITS_PER_SECTOR);
      u32 bit = i % BITS_PER_SECTOR;

      u8 buffer[BLOCK_SIZE];

      if (!disk.read_sector(sector, buffer))
        return INVALID_INODE;

      u32 byte_index = bit / 8;
      u32 bit_index = bit % 8;

      bool used = buffer[byte_index] & (1 << bit_index);

      if (!used) {
        if (!reserve_inode(i)) {
          Debugger::log("COULD NOT RESERVE INODE\n");
          return INVALID_INODE;
        }

        return i;
      }
    }

    return INVALID_INODE;
  }

  bool write_inode(u32 inode_number, Inode inode) {

    if (inode_number >= TOTAL_INODES)
      return false;

    u32 sector = INODE_TABLE_START + (inode_number / INODES_PER_SECTOR);

    u32 index = inode_number % INODES_PER_SECTOR;

    Debugger::log("WRITE INODE ");
    Debugger::log_number(inode_number);
    Debugger::log(" SECTOR ");
    Debugger::log_number(sector);
    Debugger::log(" INDEX ");
    Debugger::log_number(index);
    Debugger::log("\n");
    u8 buffer[BLOCK_SIZE];

    /*
        Read the existing inode-table sector first.
    */
    if (!disk.read_sector(sector, buffer)) {
      Debugger::log("INODE TABLE READ FAILED\n");
      return false;
    }

    Debugger::log("SECTOR BEFORE MODIFY: ");

    Debugger::log_number(buffer[0]);
    Debugger::log(" ");
    Debugger::log_number(buffer[1]);
    Debugger::log(" ");
    Debugger::log_number(buffer[2]);
    Debugger::log(" ");
    Debugger::log_number(buffer[3]);
    Debugger::log("\n");
    /*
        Modify only this inode.
    */
    Inode *table = (Inode *)buffer;

    table[index] = inode;

    /*
        Verify that the C++ structure actually made it
        into the sector buffer BEFORE touching the disk.
    */

    Debugger::log("BUFFER AFTER MODIFY: ");

    Debugger::log_number(buffer[0]);
    Debugger::log(" ");
    Debugger::log_number(buffer[1]);
    Debugger::log(" ");
    Debugger::log_number(buffer[2]);
    Debugger::log(" ");
    Debugger::log_number(buffer[3]);
    Debugger::log("\n");

    Debugger::log("BUFFER INODE: ");

    Debugger::log_number(table[index].id);

    Debugger::log(" USED=");
    Debugger::log_number(table[index].used);

    Debugger::log(" DIR=");
    Debugger::log_number(table[index].is_directory);

    Debugger::log("\n");
    /*
        Now actually write the inode-table sector.
    */
    if (!disk.write_sector(sector, buffer)) {
      Debugger::log("INODE TABLE WRITE FAILED\n");
      return false;
    }

    Debugger::log("INODE TABLE WRITE RETURNED OK\n");

    /*
        Immediately read sector 7 back.
    */
    u8 verify_buffer[BLOCK_SIZE];

    if (!disk.read_sector(sector, verify_buffer)) {
      Debugger::log("INODE VERIFY READ FAILED\n");
      return false;
    }

    Debugger::log("SECTOR AFTER WRITE: ");

    Debugger::log_number(verify_buffer[0]);
    Debugger::log(" ");

    Debugger::log_number(verify_buffer[1]);
    Debugger::log(" ");

    Debugger::log_number(verify_buffer[2]);
    Debugger::log(" ");

    Debugger::log_number(verify_buffer[3]);

    Debugger::log("\n");
    Inode *verify_table = (Inode *)verify_buffer;

    Debugger::log("VERIFY INODE: ");

    Debugger::log_number(verify_table[index].id);

    Debugger::log(" USED=");
    Debugger::log_number(verify_table[index].used);

    Debugger::log(" DIR=");
    Debugger::log_number(verify_table[index].is_directory);

    Debugger::log("\n");
    return true;
  }
  bool read_inode(u32 inode_number, Inode &out) {

    if (inode_number >= TOTAL_INODES)
      return false;

    u32 sector = INODE_TABLE_START + (inode_number / INODES_PER_SECTOR);
    u32 index = inode_number % INODES_PER_SECTOR;

    Debugger::log("READ INODE ");
    Debugger::log_number(inode_number);
    Debugger::log(" SECTOR ");
    Debugger::log_number(sector);
    Debugger::log(" INDEX ");
    Debugger::log_number(index);
    Debugger::log("\n");
    u8 buffer[BLOCK_SIZE];

    if (!disk.read_sector(sector, buffer))
      return false;

    Inode *table = (Inode *)buffer;

    out = table[index];

    return true;
  }

  bool inode_used(u32 inode_number) {

    Inode inode{};

    if (!read_inode(inode_number, inode))
      return false;

    return inode.used;
  }
};
