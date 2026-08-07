#include "../utils/fs_utils.hpp"
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

    inode_manager.mark_used(root);

    Inode inode{};

    inode.id = root;
    inode.size = 0;
    inode.is_directory = true;
    inode.parent_inode = root;
    inode.used = true;

    for (int i = 0; i < DIRECT_BLOCKS; i++) {
      inode.direct_blocks[i] = INVALID_BLOCK;
    }

    inode_manager.write_inode(root, inode);
  }

  void format() {
    block_manager.format();
    inode_manager.format();

    write_superblock();

    create_root_directory();
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

  bool delete_file(const char *path) {

  };
  bool write_file(const char *path, u8 *buffer, size_t size) {};
  bool read_file(const char *path, u8 &out);

  const char *list_directory(char *path) {
    u32 inode_number = path_resolver.resolve_path(path);

    if (inode_number == INVALID_INODE)
      return "path does not exist";

    Inode inode;
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

  const char* get_path(u32 inode_number) {
    return path_resolver.get_path(inode_number);
  }
};
