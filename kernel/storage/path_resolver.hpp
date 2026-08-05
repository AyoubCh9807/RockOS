#pragma once

#include "../shared/types.hpp"
#include "../utils/string_utils.hpp"
#include "block_manager.hpp"
#include "directory_manager.hpp"
#include "fs_types.hpp"
#include "inode_manager.hpp"
#include "layout.hpp"

class PathResolver {
private:
  InodeManager &inode_manager;
  BlockManager &block_manager;
  DirectoryManager &directory_manager;

public:
  PathResolver(InodeManager &inode_manager, BlockManager &block_manager,
               DirectoryManager &directory_manager)
      : inode_manager(inode_manager), block_manager(block_manager),
        directory_manager(directory_manager) {};

  u32 resolve_path(char *path) {
    char *args[16];
    u32 count = StringUtils::split_by(path, '/', args, 16);

    DirectoryEntry out;

    u32 current_inode_number = ROOT_INODE;
    for (auto i{0uz}; i < count; i++) {
      if (args[i][0] == '\0')
        continue;
      bool found =
          directory_manager.find_entry(current_inode_number, args[i], out);
      if (!found)
        return INVALID_INODE;
      Inode inode;
      bool is_read = inode_manager.read_inode(out.inode_number, inode);
      if (!is_read)
        return INVALID_INODE;
      bool is_last_component = i == count - 1;
      if (!is_last_component && !inode.is_directory)
        return INVALID_INODE;
      current_inode_number = out.inode_number;
    }
    return current_inode_number;

    // /home/ayoubch/notes/damian.txt
    // home, ayoubch, notes, damian.txt
  }

  int resolve_parent(const char *path);
};
