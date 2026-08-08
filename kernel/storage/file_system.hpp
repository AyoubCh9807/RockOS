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
    sb->total_blocks = TOTAL_BLOCKS;
    sb->total_inodes = TOTAL_INODES;
    sb->inode_bitmap_start = INODE_BITMAP_START;
    sb->block_bitmap_start = BLOCK_BITMAP_START;
    sb->inode_table_start = INODE_TABLE_START;
    sb->data_block_start = DATA_BLOCK_START;
    sb->size = 0;

    TerminalUtils::print("SB WRITE 1\n");

    if (!disk.write_sector(SUPERBLOCK_START, buffer)) {
      TerminalUtils::print("SB DISK WRITE FAILED\n");
      return false;
    }

    TerminalUtils::print("SB WRITE 2\n");

    u8 raw[BLOCK_SIZE] = {};

    if (!disk.read_sector(SUPERBLOCK_START, raw)) {
      TerminalUtils::print("SB DISK READ FAILED\n");
      return false;
    }

    TerminalUtils::print("SB WRITE 3\n");

    // Do NOT cast/read the entire structure yet.
    // Just inspect the first four bytes.
    TerminalUtils::print("SB RAW: ");
    TerminalUtils::print_number(raw[0]);
    TerminalUtils::print(" ");
    TerminalUtils::print_number(raw[1]);
    TerminalUtils::print(" ");
    TerminalUtils::print_number(raw[2]);
    TerminalUtils::print(" ");
    TerminalUtils::print_number(raw[3]);
    TerminalUtils::print("\n");

    TerminalUtils::print("SB WRITE 4\n");

    u32 magic = ((u32)raw[0]) | ((u32)raw[1] << 8) | ((u32)raw[2] << 16) |
                ((u32)raw[3] << 24);

    TerminalUtils::print("SB MAGIC: ");
    TerminalUtils::print_number(magic);
    TerminalUtils::print("\n");

    if (magic != FS_MAGIC) {
      TerminalUtils::print("SB MAGIC BAD\n");
      return false;
    }

    TerminalUtils::print("SB WRITE SUCCESS\n");

    return true;
  }
  bool create_root_directory() {

    u32 root = ROOT_INODE;

    if (FS_DEBUG)
      TerminalUtils::print("CREATE ROOT\n");

    // Reserve inode 0.
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

    // Allocate the first directory data block.
    u32 block = block_manager.allocate_block();

    if (block == INVALID_BLOCK) {
      TerminalUtils::print("allocate_block(root) FAILED\n");

      inode_manager.free_inode(root);
      return false;
    }

    inode.direct_blocks[0] = block;

    // Empty root directory block.
    u8 empty[BLOCK_SIZE] = {};

    if (!block_manager.write_block(block, empty)) {

      TerminalUtils::print("write_block(rootdir) FAILED\n");

      block_manager.free_block(block);
      inode_manager.free_inode(root);

      return false;
    }

    if (FS_DEBUG) {
      TerminalUtils::print("ROOT BEFORE WRITE\n");

      TerminalUtils::print("id=");
      TerminalUtils::print_number(inode.id);

      TerminalUtils::print(" used=");
      TerminalUtils::print_number(inode.used);

      TerminalUtils::print(" dir=");
      TerminalUtils::print_number(inode.is_directory);

      TerminalUtils::print(" block=");
      TerminalUtils::print_number(inode.direct_blocks[0]);

      TerminalUtils::print("\n");
    }

    // Write inode 0 to the inode table.
    if (!inode_manager.write_inode(root, inode)) {

      TerminalUtils::print("write_inode(root) FAILED\n");

      block_manager.free_block(block);
      inode_manager.free_inode(root);

      return false;
    }

    // Verify immediately
    Inode verify{};

    TerminalUtils::print("ROOT VERIFY READ START\n");

    if (!inode_manager.read_inode(ROOT_INODE, verify)) {
      TerminalUtils::print("ROOT VERIFY READ FAILED\n");
      return false;
    }

    TerminalUtils::print("ROOT VERIFY READ DONE\n");

    TerminalUtils::print("ROOT VERIFY:\n");

    TerminalUtils::print("id=");
    TerminalUtils::print_number(verify.id);

    TerminalUtils::print(" size=");
    TerminalUtils::print_number(verify.size);

    TerminalUtils::print(" used=");
    TerminalUtils::print_number(verify.used);

    TerminalUtils::print(" dir=");
    TerminalUtils::print_number(verify.is_directory);

    TerminalUtils::print(" parent=");
    TerminalUtils::print_number(verify.parent_inode);

    TerminalUtils::print(" block=");
    TerminalUtils::print_number(verify.direct_blocks[0]);

    TerminalUtils::print("\n");

    if (!verify.used) {
      TerminalUtils::print("ROOT VERIFY USED FALSE\n");
      return false;
    }

    if (!verify.is_directory) {
      TerminalUtils::print("ROOT VERIFY DIR FALSE\n");
      return false;
    }

    TerminalUtils::print("ROOT VERIFY SUCCESS\n");

    return true; }
    bool format() {

      if (!block_manager.format())
        return false;

      if (!inode_manager.format())
        return false;

      if (!write_superblock())
        return false;

      // *** BUG FIX: root inode was never created, so directory_manager saw
      // inode 0 as "unused" and every mkdir/touch/rm/rmdir at the root
      // silently failed unless a stale disk image already had root set up. ***
      if (!create_root_directory()) {
        TerminalUtils::print("create_root_directory FAILED\n");
        return false;
      }

      u8 buffer[BLOCK_SIZE] = {};

      disk.read_sector(0, buffer);

      SuperBlock *sb = (SuperBlock *)buffer;

      if (FS_DEBUG) {
        TerminalUtils::print("BEFORE ROOT MAGIC: ");
        TerminalUtils::print_number(sb->magic);
        TerminalUtils::print("\n");
      }
      return true;
    }

    bool mkdir(char *path, u32 base_dir) {
      return create(path, true, base_dir);
    }
    bool touch(char *path, u32 base_dir) {
      return create(path, false, base_dir);
    }

    bool create(char *path, bool directory, u32 base_dir) {

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

      u32 parent = path_resolver.resolve_parent(path_copy, base_dir);
      if (parent == INVALID_INODE)
        return false;

      DirectoryEntry existing{};
      if (directory_manager.find_entry(parent, name, existing))
        return false;

      u32 inode_number = inode_manager.allocate_inode();

      if (FS_DEBUG) {
        TerminalUtils::print("ALLOCATED INODE: ");
        TerminalUtils::print_number(inode_number);
        TerminalUtils::print("\n");
      }
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

    const char *list_directory(char *path, u32 base_dir) {
      if (!path)
        return "path null";
      char tmp[256];
      copy_path(tmp, path);
      u32 inode_number = path_resolver.resolve_path(tmp, base_dir);
      return list_directory(inode_number);
    }

    u32 resolve_path(char *path, u32 base_dir = ROOT_INODE) {
      return path_resolver.resolve_path(path, base_dir);
    }

    const char *get_path(u32 inode_number) {
      return path_resolver.get_path(inode_number);
    }

    bool delete_file(char *path, u32 base_dir) {
      if (!path)
        return false;

      char parent_copy[256];
      copy_path(parent_copy, path);

      char *name = FSUtils::basename(path);
      u32 parent = path_resolver.resolve_parent(parent_copy, base_dir);
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

    bool remove_directory(char *path, u32 base_dir) {

      if (FS_DEBUG)
        TerminalUtils::print("RMDIR START\n");

      if (!path)
        return false;

      char parent_copy[256];
      copy_path(parent_copy, path);

      char *name = FSUtils::basename(path);

      if (FS_DEBUG) {
        TerminalUtils::print("NAME: ");
        TerminalUtils::print(name);
        TerminalUtils::print("\n");
      }

      u32 parent = path_resolver.resolve_parent(parent_copy, base_dir);

      if (FS_DEBUG) {
        TerminalUtils::print("PARENT: ");
        TerminalUtils::print_number(parent);
        TerminalUtils::print("\n");
      }

      if (parent == INVALID_INODE)
        return false;

      DirectoryEntry entry{};

      if (FS_DEBUG)
        TerminalUtils::print("BEFORE FIND\n");

      bool found = directory_manager.find_entry(parent, name, entry);

      if (FS_DEBUG)
        TerminalUtils::print("AFTER FIND\n");

      if (!found)
        return false;

      if (FS_DEBUG) {
        TerminalUtils::print("ENTRY INODE: ");
        TerminalUtils::print_number(entry.inode_number);
        TerminalUtils::print("\n");
      }

      Inode inode{};

      if (FS_DEBUG)
        TerminalUtils::print("BEFORE READ INODE\n");

      bool read = inode_manager.read_inode(entry.inode_number, inode);

      if (FS_DEBUG)
        TerminalUtils::print("AFTER READ INODE\n");

      if (!read)
        return false;

      // Check directory is empty
      for (int i = 0; i < DIRECT_BLOCKS; i++) {

        if (inode.direct_blocks[i] == INVALID_BLOCK)
          continue;

        u8 buffer[BLOCK_SIZE];

        if (!block_manager.read_block(inode.direct_blocks[i], buffer))
          return false;

        DirectoryEntry *entries = (DirectoryEntry *)buffer;

        for (u32 j = 0; j < DIRECTORY_ENTRIES_PER_BLOCK; j++) {
          if (entries[j].is_used) {
            if (FS_DEBUG)
              TerminalUtils::print("DIRECTORY NOT EMPTY\n");
            return false;
          }
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

      if (!inode_manager.free_inode(entry.inode_number))
        return false;

      return true;
    }

    bool write_file(char *path, const u8 *buffer, size_t size, u32 base_dir) {
      if (!path || !buffer)
        return false;

      char tmp[256];
      copy_path(tmp, path);

      u32 inode_number = path_resolver.resolve_path(tmp, base_dir);
      if (inode_number == INVALID_INODE) {
        char create_path[256];
        copy_path(create_path, path);
        if (!create(create_path, false, base_dir))
          return false;

        copy_path(tmp, path);
        inode_number = path_resolver.resolve_path(tmp, base_dir);
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
                   size_t &bytes_read, u32 base_dir) {
      bytes_read = 0;
      if (!path || !buffer)
        return false;

      char tmp[256];
      copy_path(tmp, path);

      u32 inode_number = path_resolver.resolve_path(tmp, base_dir);
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

      // Extra safety: even if magic matches, make sure root is actually
      // usable (guards against disk images written by the old buggy format()).
      Inode root{};
      if (!inode_manager.read_inode(ROOT_INODE, root) || !root.used ||
          !root.is_directory) {
        TerminalUtils::print(
            "Filesystem magic OK but root invalid, reformat needed\n");
        return false;
      }

      TerminalUtils::print("Filesystem mounted\n");

      return true;
    }
  };
