#pragma once

#include "../memory/heap.hpp"
#include "../shared/types.hpp"
#include "../utils/string_utils.hpp"
#include "block_manager.hpp"
#include "inode_manager.hpp"
#include "layout.hpp"

class DirectoryManager {
private:
  InodeManager &inode_manager;
  BlockManager &block_manager;

public:
  DirectoryManager(InodeManager &inode_manager, BlockManager &block_manager)
      : inode_manager(inode_manager), block_manager(block_manager) {}

  bool find_entry(u32 directory_inode, const char *name, DirectoryEntry &out) {
    if (!name)
      return false;
    if (directory_inode >= TOTAL_INODES)
      return false;

    Inode dir_inode{};
    if (!inode_manager.read_inode(directory_inode, dir_inode))
      return false;
    if (!dir_inode.used || !dir_inode.is_directory)
      return false;

    u8 *buf = (u8 *)kmalloc(BLOCK_SIZE);
    if (!buf)
      return false;

    for (u32 i = 0; i < DIRECT_BLOCKS; i++) {
      u32 block = dir_inode.direct_blocks[i];
      if (block == INVALID_BLOCK)
        continue;

      if (!block_manager.read_block(block, buf)) {
        kfree(buf);
        return false;
      }

      DirectoryEntry *entries = (DirectoryEntry *)buf;
      for (u32 j = 0; j < DIRECTORY_ENTRIES_PER_BLOCK; j++) {
        if (!entries[j].is_used)
          continue;
        if (StringUtils::strcmp(entries[j].name, name) == 0) {
          out = entries[j];
          kfree(buf);
          return true;
        }
      }
    }

    kfree(buf);
    return false;
  }

  bool remove_entry(u32 directory_inode, const char *name) {
    if (!name)
      return false;
    if (directory_inode >= TOTAL_INODES)
      return false;

    Inode dir_inode{};
    if (!inode_manager.read_inode(directory_inode, dir_inode))
      return false;
    if (!dir_inode.used || !dir_inode.is_directory)
      return false;

    u8 *buf = (u8 *)kmalloc(BLOCK_SIZE);
    if (!buf)
      return false;

    for (u32 i = 0; i < DIRECT_BLOCKS; i++) {
      u32 block = dir_inode.direct_blocks[i];
      if (block == INVALID_BLOCK)
        continue;

      if (!block_manager.read_block(block, buf)) {
        kfree(buf);
        return false;
      }

      DirectoryEntry *entries = (DirectoryEntry *)buf;
      for (u32 j = 0; j < DIRECTORY_ENTRIES_PER_BLOCK; j++) {
        if (!entries[j].is_used)
          continue;
        if (StringUtils::strcmp(entries[j].name, name) == 0) {
          entries[j].is_used = false;
          entries[j].inode_number = 0;
          entries[j].name[0] = '\0';

          bool ok = block_manager.write_block(block, buf);
          kfree(buf);
          return ok;
        }
      }
    }

    kfree(buf);
    return false;
  }

  // Given a directory inode and a child inode number, find the entry name.
  bool find_name_by_inode(u32 directory_inode, u32 child_inode,
                          char out_name[MAX_FILENAME_LENGTH]) {
    if (!out_name)
      return false;
    out_name[0] = '\0';

    if (directory_inode >= TOTAL_INODES)
      return false;

    Inode dir_inode{};
    if (!inode_manager.read_inode(directory_inode, dir_inode))
      return false;
    if (!dir_inode.used || !dir_inode.is_directory)
      return false;

    u8 *buf = (u8 *)kmalloc(BLOCK_SIZE);
    if (!buf)
      return false;

    for (u32 i = 0; i < DIRECT_BLOCKS; i++) {
      u32 block = dir_inode.direct_blocks[i];
      if (block == INVALID_BLOCK)
        continue;

      if (!block_manager.read_block(block, buf)) {
        kfree(buf);
        return false;
      }

      DirectoryEntry *entries = (DirectoryEntry *)buf;
      for (u32 j = 0; j < DIRECTORY_ENTRIES_PER_BLOCK; j++) {
        if (!entries[j].is_used)
          continue;
        if (entries[j].inode_number == child_inode) {
          StringUtils::strcpy(out_name, entries[j].name);
          kfree(buf);
          return true;
        }
      }
    }

    kfree(buf);
    return false;
  }

  bool add_entry(u32 directory_inode, char *name, u32 inode_number) {

    Debugger::log("ADDING ENTRY ");
    Debugger::log(name);
    Debugger::log(" inode ");
    Debugger::log_number(inode_number);
    Debugger::log("\n");
    if (!name)
      return false;
    if (directory_inode >= TOTAL_INODES)
      return false;
    if (inode_number >= TOTAL_INODES)
      return false;

    // must fit inside DirectoryEntry.name INCLUDING null terminator
    if ((u32)StringUtils::strlen(name) >= MAX_FILENAME_LENGTH)
      return false;

    Inode dir_inode{};
    if (!inode_manager.read_inode(directory_inode, dir_inode))
      return false;
    if (!dir_inode.used || !dir_inode.is_directory)
      return false;

    // Reject duplicates
    DirectoryEntry existing{};
    if (find_entry(directory_inode, name, existing))
      return false;

    u8 *buf = (u8 *)kmalloc(BLOCK_SIZE);
    if (!buf)
      return false;

    // Track first free slot by location (block + index)
    bool have_free_slot = false;
    u32 free_block = INVALID_BLOCK;
    u32 free_index = 0;

    for (u32 i = 0; i < DIRECT_BLOCKS; i++) {
      u32 block = dir_inode.direct_blocks[i];
      if (block == INVALID_BLOCK)
        continue;

      if (!block_manager.read_block(block, buf)) {
        kfree(buf);
        return false;
      }

      DirectoryEntry *entries = (DirectoryEntry *)buf;
      for (u32 j = 0; j < DIRECTORY_ENTRIES_PER_BLOCK; j++) {
        if (!entries[j].is_used) {
          have_free_slot = true;
          free_block = block;
          free_index = j;
          goto FOUND_FREE;
        }
      }
    }

  FOUND_FREE:
    if (have_free_slot) {
      // write into that slot
      if (!block_manager.read_block(free_block, buf)) {
        kfree(buf);
        return false;
      }
      DirectoryEntry *entries = (DirectoryEntry *)buf;

      StringUtils::strcpy(entries[free_index].name, name);
      entries[free_index].inode_number = inode_number;
      entries[free_index].is_used = true;

      bool ok = block_manager.write_block(free_block, buf);
      kfree(buf);
      return ok;
    }

    // Need a brand new directory block
    u32 pointer_index = INVALID_BLOCK;
    for (u32 i = 0; i < DIRECT_BLOCKS; i++) {
      if (dir_inode.direct_blocks[i] == INVALID_BLOCK) {
        pointer_index = i;
        break;
      }
    }
    if (pointer_index == INVALID_BLOCK) {
      kfree(buf);
      return false;
    }

    u32 new_block = block_manager.allocate_block();
    if (new_block == INVALID_BLOCK) {
      kfree(buf);
      return false;
    }

    // Zero the block and put entry 0
    memset(buf, 0, BLOCK_SIZE);
    DirectoryEntry *entries = (DirectoryEntry *)buf;
    entries[0].is_used = true;
    StringUtils::strcpy(entries[0].name, name);
    entries[0].inode_number = inode_number;

    if (!block_manager.write_block(new_block, buf)) {
      // roll back allocation
      block_manager.free_block(new_block);
      kfree(buf);
      return false;
    }

    dir_inode.direct_blocks[pointer_index] = new_block;
    if (!inode_manager.write_inode(directory_inode, dir_inode)) {
      // roll back
      block_manager.free_block(new_block);
      kfree(buf);
      return false;
    }

    kfree(buf);
    return true;
  }
};
