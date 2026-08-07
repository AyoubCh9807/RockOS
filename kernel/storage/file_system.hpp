#pragma once

#include "../utils/fs_utils.hpp"
#include "../utils/terminal_utils.hpp"
#include "block_manager.hpp"
#include "directory_manager.hpp"
#include "inode_manager.hpp"
#include "layout.hpp"
#include "path_resolver.hpp"

class FileSystem {
private:
  Disk &disk;

  BlockManager block_manager;
  InodeManager inode_manager;
  DirectoryManager directory_manager;
  PathResolver path_resolver;

public:
  FileSystem(Disk &disk)
      : disk(disk), block_manager(disk), inode_manager(disk),
        directory_manager(inode_manager, block_manager),
        path_resolver(inode_manager, block_manager, directory_manager) {}

  void write_superblock() {
    u8 buffer[BLOCK_SIZE];

    for (int i = 0; i < BLOCK_SIZE; i++)
      buffer[i] = 0;

    SuperBlock *sb = (SuperBlock *)buffer;

    sb->magic = 0x524F434B;
    sb->size = BLOCK_SIZE;
    sb->total_blocks = TOTAL_BLOCKS;
    sb->total_inodes = TOTAL_INODES;

    disk.write_sector(SUPERBLOCK_START, buffer);
  }

  void create_root_directory() {
    u32 root = ROOT_INODE;

    bool reserved = inode_manager.reserve_inode(root);
    TerminalUtils::print(reserved ? "reserve_inode OK\n"
                                  : "reserve_inode FAILED\n");

    Inode inode{};
    inode.id = root;
    inode.size = 0;
    inode.is_directory = true;
    inode.parent_inode = root;
    inode.used = true;
    for (int i = 0; i < DIRECT_BLOCKS; i++)
      inode.direct_blocks[i] = INVALID_BLOCK;

    u32 block = block_manager.allocate_block();
    TerminalUtils::print(block == INVALID_BLOCK ? "allocate_block FAILED\n"
                                                : "allocate_block OK\n");

    if (block == INVALID_BLOCK)
      return;

    inode.direct_blocks[0] = block;

    u8 empty[BLOCK_SIZE] = {};

    TerminalUtils::print(inode.is_directory ? "CALLER INODE DIR\n"
                                            : "CALLER INODE FILE\n");

    bool wrote_block = block_manager.write_block(block, empty);
    if (!wrote_block) {
      TerminalUtils::print("Failed to write root directory block\n");
    }

    bool wrote_inode = inode_manager.write_inode(root, inode);
    if (!wrote_inode) {
      TerminalUtils::print(
          StringUtils::format("Failed to write inode {}", inode.id));
    }

    Inode debug{};
    inode_manager.read_inode(root, debug);
    TerminalUtils::print(
        debug.is_directory ? "READBACK OF ROOT AFTER WRITE ATTEMPT: DIR\n"
                           : "READBACK OF ROOT AFTER WRITE ATTEMPT: NOT DIR\n");
  }
  bool format() {
    if (!block_manager.format()) {
      TerminalUtils::print("block_manager.format FAILED\n");
      return false;
    }
    if (!inode_manager.format()) {
      TerminalUtils::print("inode_manager.format FAILED\n");
      return false;
    }

    write_superblock();

    create_root_directory();
    return true;
  }
  bool create(char *path, bool directory) {

    if (path == nullptr)
      return false;

    char *name = FSUtils::basename(path);

    size_t name_length = StringUtils::strlen(name);

    if (name_length == 0 || name_length > MAX_FILENAME_LENGTH ||
        StringUtils::contains(name, '/')) {
      return false;
    }

    // Find parent first
    u32 parent = path_resolver.resolve_parent(path);

    if (parent == INVALID_INODE)
      return false;

    // Prevent duplicate names
    DirectoryEntry existing;

    if (directory_manager.find_entry(parent, name, existing)) {
      return false;
    }

    // Allocate inode
    u32 inode_number = inode_manager.allocate_inode();

    if (inode_number == INVALID_INODE)
      return false;

    Inode node{};

    node.id = inode_number;
    node.size = 0;
    node.is_directory = directory;
    node.parent_inode = parent;
    node.used = true;

    // Mark all blocks unused
    for (int i = 0; i < DIRECT_BLOCKS; i++) {
      node.direct_blocks[i] = INVALID_BLOCK;
    }

    // Directories need a block to store DirectoryEntries
    u32 directory_block = INVALID_BLOCK;

    if (directory) {

      directory_block = block_manager.allocate_block();

      if (directory_block == INVALID_BLOCK) {
        inode_manager.free_inode(inode_number);
        return false;
      }

      node.direct_blocks[0] = directory_block;

      // Empty directory block
      u8 empty_block[BLOCK_SIZE] = {};

      if (!block_manager.write_block(directory_block, empty_block)) {

        block_manager.free_block(directory_block);
        inode_manager.free_inode(inode_number);

        return false;
      }
    }

    // Store inode on disk
    if (!inode_manager.write_inode(inode_number, node)) {

      if (directory_block != INVALID_BLOCK)
        block_manager.free_block(directory_block);

      inode_manager.free_inode(inode_number);

      return false;
    }

    // Add directory entry to parent
    if (!directory_manager.add_entry(parent, name, inode_number)) {

      inode_manager.free_inode(inode_number);

      if (directory_block != INVALID_BLOCK)
        block_manager.free_block(directory_block);

      return false;
    }

    return true;
  }

  bool mkdir(char *path) { return create(path, true); }

  const char *list_directory(char *path) {
    u32 inode_number = path_resolver.resolve_path(path);
    Inode inode;

    if (!inode_manager.read_inode(inode_number, inode))
      return "read failed";

    if (!inode.used)
      return "inode unused";

    if (!inode.is_directory)
      return "inode is file";

    if (inode_number == INVALID_INODE)
      return "path does not exist";

    //     Inode inode;
    if (!inode_manager.read_inode(inode_number, inode))
      return StringUtils::format("file {} does not exist.", path);
    if (!inode.is_directory)
      return "";
    char *big_str = (char *)kmalloc(LIST_BUFFER_SIZE);

    if (!big_str)
      return "out of memory";

    big_str[0] = '\0';

    for (int i = 0; i < DIRECT_BLOCKS; i++) {
      if (inode.direct_blocks[i] == INVALID_BLOCK)
        continue;
      // read block
      u8 buffer[BLOCK_SIZE];
      if (!block_manager.read_block(inode.direct_blocks[i], buffer))
        continue;
      DirectoryEntry *entries = reinterpret_cast<DirectoryEntry *>(buffer);
      for (int j = 0; j < DIRECTORY_ENTRIES_PER_BLOCK; j++) {
        if (!entries[j].is_used)
          continue;
        StringUtils::append(big_str, entries[j].name);
      }
    }

    return big_str;
  }

  u32 resolve_path(char *path) { return path_resolver.resolve_path(path); }

  const char *list_directory(u32 inode_number) {
    Inode inode;

    if (!inode_manager.read_inode(inode_number, inode))
      return "invalid inode";

    if (!inode.is_directory)
      return "not a directory";

    char *output = (char *)kmalloc(LIST_BUFFER_SIZE);

    if (!output)
      return "out of memory";

    output[0] = '\0';

    for (int i = 0; i < DIRECT_BLOCKS; i++) {

      if (inode.direct_blocks[i] == INVALID_BLOCK)
        continue;

      u8 buffer[BLOCK_SIZE];

      if (!block_manager.read_block(inode.direct_blocks[i], buffer))
        continue;

      DirectoryEntry *entries = reinterpret_cast<DirectoryEntry *>(buffer);

      for (int j = 0; j < DIRECTORY_ENTRIES_PER_BLOCK; j++) {

        if (!entries[j].is_used)
          continue;

        StringUtils::append(output, entries[j].name);
        StringUtils::append(output, "\n");
      }
    }

    return output;
  }

  const char *get_path(u32 inode_number) {
    return path_resolver.get_path(inode_number);
  }

  bool delete_file(char *path) {
    if (path == nullptr)
      return false;

    char *name = FSUtils::basename(path);

    u32 parent = path_resolver.resolve_parent(path);
    if (parent == INVALID_INODE)
      return false;

    DirectoryEntry entry;
    if (!directory_manager.find_entry(parent, name, entry))
      return false;

    Inode inode;
    if (!inode_manager.read_inode(entry.inode_number, inode))
      return false;

    if (inode.is_directory)
      return false; // use remove_directory for directories

    for (int i = 0; i < DIRECT_BLOCKS; i++) {
      if (inode.direct_blocks[i] != INVALID_BLOCK)
        block_manager.free_block(inode.direct_blocks[i]);
    }

    if (!directory_manager.remove_entry(parent, name))
      return false;

    return inode_manager.free_inode(entry.inode_number);
  }

  bool remove_directory(char *path) {
    if (path == nullptr)
      return false;

    char *name = FSUtils::basename(path);

    u32 parent = path_resolver.resolve_parent(path);
    if (parent == INVALID_INODE)
      return false;

    DirectoryEntry entry;
    if (!directory_manager.find_entry(parent, name, entry))
      return false;

    Inode inode;
    if (!inode_manager.read_inode(entry.inode_number, inode))
      return false;

    if (!inode.is_directory)
      return false; // use delete_file for files

    // Refuse to remove a non-empty directory
    for (int i = 0; i < DIRECT_BLOCKS; i++) {
      if (inode.direct_blocks[i] == INVALID_BLOCK)
        continue;

      u8 buffer[BLOCK_SIZE];
      if (!block_manager.read_block(inode.direct_blocks[i], buffer))
        return false;

      DirectoryEntry *entries = reinterpret_cast<DirectoryEntry *>(buffer);
      for (u32 j = 0; j < DIRECTORY_ENTRIES_PER_BLOCK; j++) {
        if (entries[j].is_used)
          return false; // not empty
      }
    }

    for (int i = 0; i < DIRECT_BLOCKS; i++) {
      if (inode.direct_blocks[i] != INVALID_BLOCK)
        block_manager.free_block(inode.direct_blocks[i]);
    }

    if (!directory_manager.remove_entry(parent, name))
      return false;

    return inode_manager.free_inode(entry.inode_number);
  }

  bool write_file(char *path, const u8 *buffer, size_t size) {
    if (path == nullptr || buffer == nullptr)
      return false;

    u32 inode_number = path_resolver.resolve_path(path);

    if (inode_number == INVALID_INODE) {
      if (!create(path, false))
        return false;
      inode_number = path_resolver.resolve_path(path);
      if (inode_number == INVALID_INODE)
        return false;
    }

    Inode inode;
    if (!inode_manager.read_inode(inode_number, inode))
      return false;

    if (inode.is_directory)
      return false;

    u32 blocks_needed = size == 0 ? 0 : (size + BLOCK_SIZE - 1) / BLOCK_SIZE;

    if (blocks_needed > DIRECT_BLOCKS)
      return false; // no indirect block support yet

    // Free whatever this inode currently owns before rewriting
    for (int i = 0; i < DIRECT_BLOCKS; i++) {
      if (inode.direct_blocks[i] != INVALID_BLOCK) {
        block_manager.free_block(inode.direct_blocks[i]);
        inode.direct_blocks[i] = INVALID_BLOCK;
      }
    }

    size_t remaining = size;
    const u8 *src = buffer;

    for (u32 i = 0; i < blocks_needed; i++) {
      u32 block = block_manager.allocate_block();
      if (block == INVALID_BLOCK)
        return false;

      u8 block_buffer[BLOCK_SIZE] = {};
      size_t chunk = remaining < BLOCK_SIZE ? remaining : BLOCK_SIZE;

      for (size_t j = 0; j < chunk; j++)
        block_buffer[j] = src[j];

      if (!block_manager.write_block(block, block_buffer))
        return false;

      inode.direct_blocks[i] = block;
      src += chunk;
      remaining -= chunk;
    }

    inode.size = size;
    return inode_manager.write_inode(inode_number, inode);
  }

  bool read_file(char *path, u8 *buffer, size_t buffer_size,
                 size_t &bytes_read) {
    bytes_read = 0;

    if (path == nullptr || buffer == nullptr)
      return false;

    u32 inode_number = path_resolver.resolve_path(path);
    if (inode_number == INVALID_INODE)
      return false;

    Inode inode;
    if (!inode_manager.read_inode(inode_number, inode))
      return false;

    if (inode.is_directory)
      return false;

    size_t to_read = inode.size < buffer_size ? inode.size : buffer_size;
    size_t remaining = to_read;
    u8 *dest = buffer;

    for (int i = 0; i < DIRECT_BLOCKS && remaining > 0; i++) {
      if (inode.direct_blocks[i] == INVALID_BLOCK)
        break;

      u8 block_buffer[BLOCK_SIZE];
      if (!block_manager.read_block(inode.direct_blocks[i], block_buffer))
        return false;

      size_t chunk = remaining < BLOCK_SIZE ? remaining : BLOCK_SIZE;
      for (size_t j = 0; j < chunk; j++)
        dest[j] = block_buffer[j];

      dest += chunk;
      remaining -= chunk;
    }

    bytes_read = to_read;
    return true;
  }

  bool touch(char *path) { return create(path, false); }
};
