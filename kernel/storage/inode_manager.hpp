#pragma once

#include "../core/kernel.hpp"
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
        return false;
      }
    }

    // Clear inode table
    u8 empty[BLOCK_SIZE] = {};

    for (u32 i = 0; i < INODE_TABLE_SECTORS; i++) {
      if (!disk.write_sector(INODE_TABLE_START + i, empty)) {
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
        if (!reserve_inode(i))
          return INVALID_INODE;

        return i;
      }
    }

    return INVALID_INODE;
  }

bool write_inode(u32 inode_number, Inode inode) {

    if (inode_number >= TOTAL_INODES)
        return false;

    u32 sector =
        INODE_TABLE_START +
        (inode_number / INODES_PER_SECTOR);

    u32 index =
        inode_number % INODES_PER_SECTOR;

    TerminalUtils::print("WRITE INODE ");
    TerminalUtils::print_number(inode_number);
    TerminalUtils::print(" SECTOR ");
    TerminalUtils::print_number(sector);
    TerminalUtils::print(" INDEX ");
    TerminalUtils::print_number(index);
    TerminalUtils::print("\n");

    u8 buffer[BLOCK_SIZE];

    /*
        Read the existing inode-table sector first.
    */
    if (!disk.read_sector(sector, buffer)) {
        TerminalUtils::print(
            "INODE TABLE READ FAILED\n");
        return false;
    }

    TerminalUtils::print(
        "SECTOR BEFORE MODIFY: "
    );

    TerminalUtils::print_number(buffer[0]);
    TerminalUtils::print(" ");
    TerminalUtils::print_number(buffer[1]);
    TerminalUtils::print(" ");
    TerminalUtils::print_number(buffer[2]);
    TerminalUtils::print(" ");
    TerminalUtils::print_number(buffer[3]);
    TerminalUtils::print("\n");

    /*
        Modify only this inode.
    */
    Inode *table = (Inode *)buffer;

    table[index] = inode;

    /*
        Verify that the C++ structure actually made it
        into the sector buffer BEFORE touching the disk.
    */
    TerminalUtils::print(
        "BUFFER AFTER MODIFY: "
    );

    TerminalUtils::print_number(buffer[0]);
    TerminalUtils::print(" ");
    TerminalUtils::print_number(buffer[1]);
    TerminalUtils::print(" ");
    TerminalUtils::print_number(buffer[2]);
    TerminalUtils::print(" ");
    TerminalUtils::print_number(buffer[3]);
    TerminalUtils::print("\n");

    TerminalUtils::print(
        "BUFFER INODE: "
    );

    TerminalUtils::print_number(
        table[index].id
    );

    TerminalUtils::print(" USED=");
    TerminalUtils::print_number(
        table[index].used
    );

    TerminalUtils::print(" DIR=");
    TerminalUtils::print_number(
        table[index].is_directory
    );

    TerminalUtils::print("\n");

    /*
        Now actually write the inode-table sector.
    */
    if (!disk.write_sector(sector, buffer)) {
        TerminalUtils::print(
            "INODE TABLE WRITE FAILED\n");
        return false;
    }

    TerminalUtils::print(
        "INODE TABLE WRITE RETURNED OK\n"
    );

    /*
        Immediately read sector 7 back.
    */
    u8 verify_buffer[BLOCK_SIZE];

    if (!disk.read_sector(sector, verify_buffer)) {
        TerminalUtils::print(
            "INODE VERIFY READ FAILED\n");
        return false;
    }

    TerminalUtils::print(
        "SECTOR AFTER WRITE: "
    );

    TerminalUtils::print_number(
        verify_buffer[0]
    );
    TerminalUtils::print(" ");

    TerminalUtils::print_number(
        verify_buffer[1]
    );
    TerminalUtils::print(" ");

    TerminalUtils::print_number(
        verify_buffer[2]
    );
    TerminalUtils::print(" ");

    TerminalUtils::print_number(
        verify_buffer[3]
    );

    TerminalUtils::print("\n");

    Inode *verify_table =
        (Inode *)verify_buffer;

    TerminalUtils::print(
        "VERIFY INODE: "
    );

    TerminalUtils::print_number(
        verify_table[index].id
    );

    TerminalUtils::print(" USED=");
    TerminalUtils::print_number(
        verify_table[index].used
    );

    TerminalUtils::print(" DIR=");
    TerminalUtils::print_number(
        verify_table[index].is_directory
    );

    TerminalUtils::print("\n");

    return true;
}
  bool read_inode(u32 inode_number, Inode &out) {

    if (inode_number >= TOTAL_INODES)
      return false;

    u32 sector = INODE_TABLE_START + (inode_number / INODES_PER_SECTOR);
    u32 index = inode_number % INODES_PER_SECTOR;

    if (FS_DEBUG) {
      TerminalUtils::print("READ INODE ");
      TerminalUtils::print_number(inode_number);
      TerminalUtils::print(" SECTOR ");
      TerminalUtils::print_number(sector);
      TerminalUtils::print(" INDEX ");
      TerminalUtils::print_number(index);
      TerminalUtils::print("\n");
    }
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
