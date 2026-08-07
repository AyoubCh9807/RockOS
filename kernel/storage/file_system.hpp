#pragma once

#include "../memory/memory.hpp"
#include "../utils/fs_utils.hpp"
#include "../utils/string_utils.hpp"
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

  static void append_bounded(char *dst, u32 cap, const char *src) {
    if (!dst || cap == 0)
      return;
    if (!src)
      src = "";
    u32 len = (u32)StringUtils::strlen(dst);
    if (len >= cap) {
      dst[cap - 1] = '\0';
      return;
    }
    u32 i = 0;
    while (src[i] != '\0' && (len + i + 1) < cap) {
      dst[len + i] = src[i];
      i++;
    }
    dst[len + i] = '\0';
  }

  static void copy_path(char out[256], const char *in) {
    int i = 0;
    if (!in) {
      out[0] = '\0';
      return;
    }
    while (in[i] != '\0' && i < 255) {
      out[i] = in[i];
      i++;
    }
    out[i] = '\0';
  }

public:
  FileSystem(Disk &disk)
      : disk(disk), block_manager(disk), inode_manager(disk),
        directory_manager(inode_manager, block_manager),
        path_resolver(inode_manager, directory_manager) {}

  bool write_superblock() {
    u8 buffer[BLOCK_SIZE] = {};

    SuperBlock *sb = (SuperBlock *)buffer;

    sb->magic = FS_MAGIC;

    TerminalUtils::print("BEFORE DISK WRITE: ");
    TerminalUtils::print_number(sb->magic);
    TerminalUtils::print("\n");

    if (!disk.write_sector(SUPERBLOCK_START, buffer)) {
      TerminalUtils::print("WRITE FAILED\n");
      return false;
    }

    u8 raw[BLOCK_SIZE] = {};

    disk.read_sector(0, raw);

    TerminalUtils::print("RAW AFTER WRITE: ");
    TerminalUtils::print_number(raw[0]);
    TerminalUtils::print(" ");
    TerminalUtils::print_number(raw[1]);
    TerminalUtils::print(" ");
    TerminalUtils::print_number(raw[2]);
    TerminalUtils::print("\n");

    return true;
  }
  bool create_root_directory() {
    u32 root = ROOT_INODE;

    if (!inode_manager.reserve_inode(root)) {
      TerminalUtils::print("reserve_inode(root) FAILED\n");
      return false;
    }

    Inode inode{};
    inode.id = root;
    inode.size = 0;
    inode.is_directory = true;
    inode.parent_inode = root;
    inode.used = true;
    for (int i = 0; i < DIRECT_BLOCKS; i++)
      inode.direct_blocks[i] = INVALID_BLOCK;
    inode.indirect_block = INVALID_BLOCK;

    u32 block = block_manager.allocate_block();
    if (block == INVALID_BLOCK) {
      TerminalUtils::print("allocate_block(root) FAILED\n");
      return false;
    }
    inode.direct_blocks[0] = block;

    u8 empty[BLOCK_SIZE] = {};
    if (!block_manager.write_block(block, empty)) {
      TerminalUtils::print("write_block(rootdir) FAILED\n");
      return false;
    }

    if (!inode_manager.write_inode(root, inode)) {
      TerminalUtils::print("write_inode(root) FAILED\n");
      return false;
    }

    // verify immediately
    Inode verify{};
    if (!inode_manager.read_inode(root, verify)) {
      TerminalUtils::print("read_inode(root verify) FAILED\n");
      return false;
    }
    if (!verify.used) {
      TerminalUtils::print("VERIFY: USED = FALSE\n");
    }

    if (!verify.is_directory) {
      TerminalUtils::print("VERIFY: DIR = FALSE\n");
    }

    TerminalUtils::print(verify.is_directory ? "VERIFY DIR\n"
                                             : "VERIFY FILE\n");
    return true;
  }

  bool format() {

    if (!block_manager.format())
      return false;

    if (!inode_manager.format())
      return false;

    if (!write_superblock())
      return false;

    u8 buffer[BLOCK_SIZE] = {};

    disk.read_sector(0, buffer);

    SuperBlock *sb = (SuperBlock *)buffer;

    TerminalUtils::print("BEFORE ROOT MAGIC: ");
    TerminalUtils::print_number(sb->magic);
    TerminalUtils::print("\n");

    return true;
  }
  bool mkdir(char *path) { return create(path, true); }
  bool touch(char *path) { return create(path, false); }

  bool create(char *path, bool directory) {
    if (!path)
      return false;

    // resolve_parent mutates, so use a copy
    char path_copy[256];
    copy_path(path_copy, path);

    char *name = FSUtils::basename(path);
    u32 name_length = (u32)StringUtils::strlen(name);
    if (name_length == 0 || name_length >= MAX_FILENAME_LENGTH ||
        StringUtils::contains(name, '/'))
      return false;

    u32 parent = path_resolver.resolve_parent(path_copy);
    if (parent == INVALID_INODE)
      return false;

    DirectoryEntry existing{};
    if (directory_manager.find_entry(parent, name, existing))
      return false;

    u32 inode_number = inode_manager.allocate_inode();
    if (inode_number == INVALID_INODE)
      return false;

    Inode node{};
    node.id = inode_number;
    node.size = 0;
    node.is_directory = directory;
    node.parent_inode = parent;
    node.used = true;
    for (int i = 0; i < DIRECT_BLOCKS; i++)
      node.direct_blocks[i] = INVALID_BLOCK;
    node.indirect_block = INVALID_BLOCK;

    u32 directory_block = INVALID_BLOCK;
    if (directory) {
      directory_block = block_manager.allocate_block();
      if (directory_block == INVALID_BLOCK) {
        inode_manager.free_inode(inode_number);
        return false;
      }
      node.direct_blocks[0] = directory_block;

      u8 empty_block[BLOCK_SIZE] = {};
      if (!block_manager.write_block(directory_block, empty_block)) {
        block_manager.free_block(directory_block);
        inode_manager.free_inode(inode_number);
        return false;
      }
    }

    if (!inode_manager.write_inode(inode_number, node)) {
      if (directory_block != INVALID_BLOCK)
        block_manager.free_block(directory_block);
      inode_manager.free_inode(inode_number);
      return false;
    }

    if (!directory_manager.add_entry(parent, name, inode_number)) {
      inode_manager.free_inode(inode_number);
      if (directory_block != INVALID_BLOCK)
        block_manager.free_block(directory_block);
      return false;
    }

    return true;
  }

  const char *list_directory(u32 inode_number) {
    if (inode_number == INVALID_INODE)
      return "invalid inode";

    Inode inode{};
    if (!inode_manager.read_inode(inode_number, inode))
      return "invalid inode";
    if (!inode.used)
      return "inode unused";
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

      DirectoryEntry *entries = (DirectoryEntry *)buffer;
      for (u32 j = 0; j < DIRECTORY_ENTRIES_PER_BLOCK; j++) {
        if (!entries[j].is_used)
          continue;
        append_bounded(output, LIST_BUFFER_SIZE, entries[j].name);
        append_bounded(output, LIST_BUFFER_SIZE, "\n");
      }
    }

    return output;
  }

  const char *list_directory(char *path) {
    if (!path)
      return "path null";
    char tmp[256];
    copy_path(tmp, path);
    u32 inode_number = path_resolver.resolve_path(tmp);
    return list_directory(inode_number);
  }

  u32 resolve_path(char *path) { return path_resolver.resolve_path(path); }
  const char *get_path(u32 inode_number) {
    return path_resolver.get_path(inode_number);
  }

  bool delete_file(char *path) {
    if (!path)
      return false;

    char parent_copy[256];
    copy_path(parent_copy, path);

    char *name = FSUtils::basename(path);
    u32 parent = path_resolver.resolve_parent(parent_copy);
    if (parent == INVALID_INODE)
      return false;

    DirectoryEntry entry{};
    if (!directory_manager.find_entry(parent, name, entry))
      return false;

    Inode inode{};
    if (!inode_manager.read_inode(entry.inode_number, inode))
      return false;
    if (!inode.used)
      return false;
    if (inode.is_directory)
      return false;

    for (int i = 0; i < DIRECT_BLOCKS; i++) {
      if (inode.direct_blocks[i] != INVALID_BLOCK) {
        block_manager.free_block(inode.direct_blocks[i]);
        inode.direct_blocks[i] = INVALID_BLOCK;
      }
    }

    if (!directory_manager.remove_entry(parent, name))
      return false;
    return inode_manager.free_inode(entry.inode_number);
  }

  bool remove_directory(char *path) {
    if (!path)
      return false;

    char parent_copy[256];
    copy_path(parent_copy, path);

    char *name = FSUtils::basename(path);
    u32 parent = path_resolver.resolve_parent(parent_copy);
    if (parent == INVALID_INODE)
      return false;

    DirectoryEntry entry{};
    if (!directory_manager.find_entry(parent, name, entry))
      return false;

    Inode inode{};
    if (!inode_manager.read_inode(entry.inode_number, inode))
      return false;
    if (!inode.used)
      return false;
    if (!inode.is_directory)
      return false;

    // must be empty
    for (int i = 0; i < DIRECT_BLOCKS; i++) {
      if (inode.direct_blocks[i] == INVALID_BLOCK)
        continue;

      u8 buffer[BLOCK_SIZE];
      if (!block_manager.read_block(inode.direct_blocks[i], buffer))
        return false;

      DirectoryEntry *entries = (DirectoryEntry *)buffer;
      for (u32 j = 0; j < DIRECTORY_ENTRIES_PER_BLOCK; j++) {
        if (entries[j].is_used)
          return false;
      }
    }

    for (int i = 0; i < DIRECT_BLOCKS; i++) {
      if (inode.direct_blocks[i] != INVALID_BLOCK) {
        block_manager.free_block(inode.direct_blocks[i]);
        inode.direct_blocks[i] = INVALID_BLOCK;
      }
    }

    if (!directory_manager.remove_entry(parent, name))
      return false;
    return inode_manager.free_inode(entry.inode_number);
  }

  bool write_file(char *path, const u8 *buffer, size_t size) {
    if (!path || !buffer)
      return false;

    char tmp[256];
    copy_path(tmp, path);

    u32 inode_number = path_resolver.resolve_path(tmp);
    if (inode_number == INVALID_INODE) {
      // create needs mutable path
      char create_path[256];
      copy_path(create_path, path);
      if (!create(create_path, false))
        return false;

      copy_path(tmp, path);
      inode_number = path_resolver.resolve_path(tmp);
      if (inode_number == INVALID_INODE)
        return false;
    }

    Inode inode{};
    if (!inode_manager.read_inode(inode_number, inode))
      return false;
    if (inode.is_directory)
      return false;

    u32 blocks_needed =
        (size == 0) ? 0 : (u32)((size + BLOCK_SIZE - 1) / BLOCK_SIZE);
    if (blocks_needed > DIRECT_BLOCKS)
      return false;

    // free existing
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

      u8 blk[BLOCK_SIZE] = {};
      size_t chunk = (remaining < BLOCK_SIZE) ? remaining : BLOCK_SIZE;
      for (size_t j = 0; j < chunk; j++)
        blk[j] = src[j];

      if (!block_manager.write_block(block, blk))
        return false;

      inode.direct_blocks[i] = block;
      src += chunk;
      remaining -= chunk;
    }

    inode.size = (u32)size;
    return inode_manager.write_inode(inode_number, inode);
  }

  bool read_file(char *path, u8 *buffer, size_t buffer_size,
                 size_t &bytes_read) {
    bytes_read = 0;
    if (!path || !buffer)
      return false;

    char tmp[256];
    copy_path(tmp, path);

    u32 inode_number = path_resolver.resolve_path(tmp);
    if (inode_number == INVALID_INODE)
      return false;

    Inode inode{};
    if (!inode_manager.read_inode(inode_number, inode))
      return false;
    if (inode.is_directory)
      return false;

    size_t to_read = (inode.size < buffer_size) ? inode.size : buffer_size;
    size_t remaining = to_read;
    u8 *dest = buffer;

    for (int i = 0; i < DIRECT_BLOCKS && remaining > 0; i++) {
      if (inode.direct_blocks[i] == INVALID_BLOCK)
        break;

      u8 blk[BLOCK_SIZE];
      if (!block_manager.read_block(inode.direct_blocks[i], blk))
        return false;

      size_t chunk = (remaining < BLOCK_SIZE) ? remaining : BLOCK_SIZE;
      for (size_t j = 0; j < chunk; j++)
        dest[j] = blk[j];

      dest += chunk;
      remaining -= chunk;
    }

    bytes_read = to_read;
    return true;
  }

  bool mount() {
    u8 buffer[BLOCK_SIZE];

    if (!disk.read_sector(SUPERBLOCK_START, buffer))
      return false;

    SuperBlock *sb = (SuperBlock *)buffer;

    if (sb->magic != FS_MAGIC) {
      TerminalUtils::print("No filesystem found\n");
      return false;
    }

    TerminalUtils::print("Filesystem mounted\n");

    return true;
  }
};
