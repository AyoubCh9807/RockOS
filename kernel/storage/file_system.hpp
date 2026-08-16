#pragma once

#include "../memory/memory.hpp"
#include "../utils/debugger.hpp"
#include "../utils/fs_utils.hpp"
#include "../utils/string_utils.hpp"
#include "../utils/terminal_utils.hpp"
#include "block_manager.hpp"
#include "directory_manager.hpp"
#include "inode_manager.hpp"
#include "layout.hpp"
#include "path_resolver.hpp"

class FileSystem {
  // private:
public:
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

  // public:
  FileSystem(Disk &disk)
      : disk(disk), block_manager(disk), inode_manager(disk),
        directory_manager(inode_manager, block_manager),
        path_resolver(inode_manager, directory_manager) {}

  // Returns true only if the inode exists, is in-use, and is a directory.
  // Callers (like cd) that must reject files use this explicitly, since
  // PathResolver::resolve_path() intentionally allows resolving to a file
  // (cat/ls/rm all need that) and can't enforce "must be a directory" on
  // its own.
  bool is_directory(u32 inode_number) {
    if (inode_number == INVALID_INODE)
      return false;
    Inode inode{};
    if (!inode_manager.read_inode(inode_number, inode))
      return false;
    if (!inode.used)
      return false;
    return inode.is_directory;
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

  bool write_superblock() {
    u8 buffer[BLOCK_SIZE] = {};

    // Just puts "ROCK" directly into the sector.
    buffer[0] = 0x4B; // K
    buffer[1] = 0x43; // C
    buffer[2] = 0x4F; // O
    buffer[3] = 0x52; // R

    Debugger::log("=== WRITE SUPERBLOCK ===\n");

    Debugger::log("TARGET SECTOR: ");
    Debugger::log_number(SUPERBLOCK_START);
    Debugger::log("\n");

    Debugger::log("BUFFER BEFORE WRITE: ");
    Debugger::log_number(buffer[0]);
    Debugger::log(" ");
    Debugger::log_number(buffer[1]);
    Debugger::log(" ");
    Debugger::log_number(buffer[2]);
    Debugger::log(" ");
    Debugger::log_number(buffer[3]);
    Debugger::log("\n");

    bool write_ok = disk.write_sector(SUPERBLOCK_START, buffer);

    Debugger::log("WRITE RESULT: ");
    Debugger::log_number(write_ok);
    Debugger::log("\n");

    u8 verify[BLOCK_SIZE] = {};

    bool read_ok = disk.read_sector(SUPERBLOCK_START, verify);

    Debugger::log("READ RESULT: ");
    Debugger::log_number(read_ok);
    Debugger::log("\n");

    Debugger::log("BUFFER AFTER READ: ");
    Debugger::log_number(verify[0]);
    Debugger::log(" ");
    Debugger::log_number(verify[1]);
    Debugger::log(" ");
    Debugger::log_number(verify[2]);
    Debugger::log(" ");
    Debugger::log_number(verify[3]);
    Debugger::log("\n");

    if (verify[0] != 0x4B || verify[1] != 0x43 || verify[2] != 0x4F ||
        verify[3] != 0x52) {

      Debugger::log("SUPERBLOCK RAW WRITE FAILED\n");
      return false;
    }

    Debugger::log("SUPERBLOCK RAW WRITE SUCCESS\n");

    return true;
  }
  bool create_root_directory() {

    u32 root = ROOT_INODE;

    Debugger::log("CREATE ROOT\n");

    // Reserve inode 0.
    if (!inode_manager.reserve_inode(root)) {
      Debugger::log("reserve_inode(root) FAILED\n");
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
      Debugger::log("allocate_block(root) FAILED\n");

      inode_manager.free_inode(root);
      return false;
    }

    inode.direct_blocks[0] = block;

    // Empty root directory block.
    u8 empty[BLOCK_SIZE] = {};

    if (!block_manager.write_block(block, empty)) {

      Debugger::log("write_block(rootdir) FAILED\n");

      block_manager.free_block(block);
      inode_manager.free_inode(root);

      return false;
    }

    Debugger::log("ROOT BEFORE WRITE\n");

    Debugger::log("id=");
    Debugger::log_number(inode.id);

    Debugger::log(" used=");
    Debugger::log_number(inode.used);

    Debugger::log(" dir=");
    Debugger::log_number(inode.is_directory);

    Debugger::log(" block=");
    Debugger::log_number(inode.direct_blocks[0]);

    Debugger::log("\n");

    // Write inode 0 to the inode table.
    if (!inode_manager.write_inode(root, inode)) {

      Debugger::log("write_inode(root) FAILED\n");

      block_manager.free_block(block);
      inode_manager.free_inode(root);

      return false;
    }

    // Verify immediately
    Inode verify{};

    Debugger::log("ROOT VERIFY READ START\n");

    if (!inode_manager.read_inode(ROOT_INODE, verify)) {
      Debugger::log("ROOT VERIFY READ FAILED\n");
      return false;
    }
    Debugger::log("ROOT VERIFY READ DONE\n");

    Debugger::log("ROOT VERIFY:\n");

    Debugger::log("id=");
    Debugger::log_number(verify.id);

    Debugger::log(" size=");
    Debugger::log_number(verify.size);

    Debugger::log(" used=");
    Debugger::log_number(verify.used);

    Debugger::log(" dir=");
    Debugger::log_number(verify.is_directory);

    Debugger::log(" parent=");
    Debugger::log_number(verify.parent_inode);

    Debugger::log(" block=");
    Debugger::log_number(verify.direct_blocks[0]);

    Debugger::log("\n");
    if (!verify.used) {
      Debugger::log("ROOT VERIFY USED FALSE\n");
      return false;
    }

    if (!verify.is_directory) {
      Debugger::log("ROOT VERIFY DIR FALSE\n");
      return false;
    }

    Debugger::log("ROOT VERIFY SUCCESS\n");

    return true;
  }
  bool format() {
    Debugger::log("=== FORMAT START ===\n");

    Debugger::log("TEST SECTOR 1 BEFORE FORMATTING\n");
    disk.test_sector_one();

    Debugger::log("CALLING BLOCK MANAGER FORMAT\n");

    if (!block_manager.format()) {
      Debugger::log("BLOCK MANAGER FORMAT FAILED\n");
      return false;
    }

    Debugger::log("TEST SECTOR 1 AFTER BLOCK FORMAT\n");
    disk.test_sector_one();

    Debugger::log("CALLING INODE MANAGER FORMAT\n");

    if (!inode_manager.format()) {
      Debugger::log("INODE MANAGER FORMAT FAILED\n");
      return false;
    }

    Debugger::log("TEST SECTOR 1 AFTER INODE FORMAT\n");
    disk.test_sector_one();

    Debugger::log("CALLING WRITE SUPERBLOCK\n");

    if (!write_superblock()) {
      Debugger::log("WRITE SUPERBLOCK FAILED\n");
      return false;
    }

    Debugger::log("SUPERBLOCK OK\n");

    if (!create_root_directory()) {
      Debugger::log("CREATE ROOT FAILED\n");
      return false;
    }

    Debugger::log("=== FORMAT SUCCESS ===\n");

    return true;
  }

  bool mkdir(char *path, u32 base_dir) { return create(path, true, base_dir); }
  bool touch(char *path, u32 base_dir) { return create(path, false, base_dir); }

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

    Debugger::log("ALLOCATED INODE: ");
    Debugger::log_number(inode_number);
    Debugger::log("\n");
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

    Debugger::log("REMOVE TARGET INODE: ");
    Debugger::log_number(entry.inode_number);
    Debugger::log("\n");

    Inode inode{};
    if (!inode_manager.read_inode(entry.inode_number, inode))
      return false;

    Debugger::log("REMOVE INODE:\n");
    Debugger::log("id=");
    Debugger::log_number(inode.id);
    Debugger::log(" used=");
    Debugger::log_number(inode.used);
    Debugger::log(" dir=");
    Debugger::log_number(inode.is_directory);
    Debugger::log(" size=");
    Debugger::log_number(inode.size);
    Debugger::log("\n");

    for (u32 i = 0; i < DIRECT_BLOCKS; i++) {
      Debugger::log("block[");
      Debugger::log_number(i);
      Debugger::log("]=");
      Debugger::log_number(inode.direct_blocks[i]);
      Debugger::log("\n");
    }

    /*
     * If this is a directory, delete_file() must not remove it.
     */
    if (inode.is_directory)
      return false;

    /*
     * IMPORTANT (reordered):
     *
     * Free the inode's data blocks and the inode itself FIRST,
     * while the directory entry still exists. This means a
     * partial failure here leaves the entry intact — the file
     * is still "there" (possibly with some blocks already freed,
     * which is logged, but never silently vanished from the
     * directory while reporting failure to the caller).
     *
     * Only remove the directory entry once we're sure everything
     * else succeeded, since that's the point of no return.
     */
    for (u32 i = 0; i < DIRECT_BLOCKS; i++) {
      if (inode.direct_blocks[i] != INVALID_BLOCK) {
        if (!block_manager.free_block(inode.direct_blocks[i])) {
          Debugger::log("FAILED FREEING BLOCK\n");
          return false;
        }
        inode.direct_blocks[i] = INVALID_BLOCK;
      }
    }

    if (!inode_manager.free_inode(entry.inode_number)) {
      Debugger::log("FAILED FREEING INODE\n");
      return false;
    }

    if (!directory_manager.remove_entry(parent, name)) {
      // Inode/blocks are already freed at this point. This is now an
      // orphaned-but-freed situation rather than a corrupt one: the
      // inode is correctly marked free, but the entry still points at
      // it. Logged for visibility since this shouldn't normally happen.
      Debugger::log("FAILED REMOVING DIRECTORY ENTRY (inode already freed)\n");
      return false;
    }

    Debugger::log("FILE REMOVED\n");

    return true;
  }

  bool remove_directory(char *path, u32 base_dir) {
    Debugger::log("RMDIR START\n");

    if (!path)
      return false;

    char parent_copy[256];
    copy_path(parent_copy, path);

    char *name = FSUtils::basename(path);

    Debugger::log("NAME: ");
    Debugger::log(name);
    Debugger::log("\n");

    u32 parent = path_resolver.resolve_parent(parent_copy, base_dir);

    Debugger::log("PARENT: ");
    Debugger::log_number(parent);
    Debugger::log("\n");

    if (parent == INVALID_INODE)
      return false;

    DirectoryEntry entry{};

    Debugger::log("BEFORE FIND\n");

    if (!directory_manager.find_entry(parent, name, entry)) {
      Debugger::log("RMDIR ENTRY NOT FOUND\n");
      return false;
    }

    Debugger::log("AFTER FIND\n");

    Debugger::log("ENTRY INODE: ");
    Debugger::log_number(entry.inode_number);
    Debugger::log("\n");

    Inode inode{};

    Debugger::log("BEFORE READ INODE\n");

    if (!inode_manager.read_inode(entry.inode_number, inode)) {
      Debugger::log("RM INODE READ FAILED\n");
      return false;
    }

    Debugger::log("RM INODE READ OK\n");

    Debugger::log(inode.is_directory ? "RM: DIR\n" : "RM: FILE\n");
    Debugger::log(inode.used ? "RM: USED\n" : "RM: NOT USED\n");

    Debugger::log(StringUtils::format("RM INODE: %d USED=%d DIR=%d SIZE=%d\n",
                                      inode.id, inode.used, inode.is_directory,
                                      inode.size));

    /*
     * The inode must actually represent a directory.
     */
    if (!inode.is_directory) {
      Debugger::log("RMDIR TARGET IS NOT A DIRECTORY\n");
      return false;
    }

    /*
     * The directory must be empty.
     */
    for (u32 i = 0; i < DIRECT_BLOCKS; i++) {

      if (inode.direct_blocks[i] == INVALID_BLOCK)
        continue;

      u8 buffer[BLOCK_SIZE];

      if (!block_manager.read_block(inode.direct_blocks[i], buffer))
        return false;

      DirectoryEntry *entries = (DirectoryEntry *)buffer;

      for (u32 j = 0; j < DIRECTORY_ENTRIES_PER_BLOCK; j++) {
        if (entries[j].is_used) {
          Debugger::log("DIRECTORY NOT EMPTY\n");
          return false;
        }
      }
    }

    /*
     * IMPORTANT (reordered):
     *
     * Free the directory's data blocks and the inode itself FIRST,
     * while the parent's entry still points at it. A failure here
     * leaves the parent entry intact instead of silently orphaning
     * it while reporting failure.
     *
     * Only remove the parent directory entry once everything else
     * has actually succeeded.
     */
    for (u32 i = 0; i < DIRECT_BLOCKS; i++) {

      if (inode.direct_blocks[i] != INVALID_BLOCK) {

        if (!block_manager.free_block(inode.direct_blocks[i])) {
          Debugger::log("FAILED FREEING DIRECTORY BLOCK\n");
          return false;
        }

        inode.direct_blocks[i] = INVALID_BLOCK;
      }
    }

    if (!inode_manager.free_inode(entry.inode_number)) {
      Debugger::log("FAILED FREEING DIRECTORY INODE\n");
      return false;
    }

    if (!directory_manager.remove_entry(parent, name)) {
      // Inode/blocks already freed. Orphaned entry, logged for
      // visibility; shouldn't normally happen.
      Debugger::log("FAILED REMOVING DIRECTORY ENTRY (inode already freed)\n");
      return false;
    }

    Debugger::log("DIRECTORY REMOVED\n");

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
    if (inode.is_directory) {
      return false;
    }
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

  bool read_file(char *path, u8 *buffer, size_t buffer_size, size_t &bytes_read,
                 u32 base_dir) {
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

    Debugger::log("=== MOUNT START ===\n");

    if (!disk.read_sector(SUPERBLOCK_START, buffer)) {
      Debugger::log("MOUNT: SUPERBLOCK READ FAILED\n");
      return false;
    }

    Debugger::log("SUPERBLOCK RAW: ");
    Debugger::log_number(buffer[0]);
    Debugger::log(" ");
    Debugger::log_number(buffer[1]);
    Debugger::log(" ");
    Debugger::log_number(buffer[2]);
    Debugger::log(" ");
    Debugger::log_number(buffer[3]);
    Debugger::log("\n");

    SuperBlock *sb = (SuperBlock *)buffer;

    if (sb->magic != FS_MAGIC) {
      Debugger::log("MOUNT: MAGIC INVALID\n");
      return false;
    }

    Inode root{};

    if (!inode_manager.read_inode(ROOT_INODE, root)) {
      Debugger::log("MOUNT: ROOT READ FAILED\n");
      return false;
    }

    Debugger::log("MOUNT ROOT: ");
    Debugger::log("id=");
    Debugger::log_number(root.id);
    Debugger::log(" used=");
    Debugger::log_number(root.used);
    Debugger::log(" dir=");
    Debugger::log_number(root.is_directory);
    Debugger::log("\n");

    if (!root.used || !root.is_directory) {
      Debugger::log("MOUNT: ROOT INVALID\n");
      return false;
    }

    Debugger::log("MOUNT SUCCESS\n");

    return true;
  }
};
