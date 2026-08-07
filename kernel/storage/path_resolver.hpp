#pragma once

#include "../shared/types.hpp"
#include "../utils/string_utils.hpp"
#include "block_manager.hpp"
#include "directory_manager.hpp"
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
    auto count = StringUtils::split_by(path, '/', args, 16);

    if (count == 0)
      return ROOT_INODE;

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

  int resolve_parent(char *path) {
    // /home/ayoubch/damian.txt becomes /home/ayoubch/
    // find last "/" char and replace it with \0

    if (path == nullptr)
      return INVALID_INODE;
    if (path[0] == '/') {
      if (path[1] == '\0')
        return ROOT_INODE;
    }

    if (StringUtils::strlen(path) == 0)
      return INVALID_INODE;

    int index = -1;

    int path_length = StringUtils::strlen(path);
    for (int i = path_length - 1; i >= 0; i++) {
      if (path[i] == '/') {
        index = i;
        break;
      }
    }
    if (index == 0)
      return ROOT_INODE;

    // For relative paths later on, i can use this
    // if(!found) return resolve_path(path);
    if (index == -1)
      return INVALID_INODE;
    path[index] = '\0';

    return resolve_path(path);

    // becomes /home/ayoubch\0damian.txt\0
    // path gets from beginning to null terminator
    // call resolve_path on path
    // return result
  };

  const char *get_path(u32 inode_number) {

    if (inode_number == INVALID_INODE)
      return "invalid inode";

    char *path = (char *)kmalloc(256);

    if (!path)
      return "out of memory";

    path[0] = '\0';

    char names[16][MAX_FILENAME_LENGTH];

    int depth = 0;

    u32 current = inode_number;

    while (current != ROOT_INODE && depth < 16) {

      Inode inode;

      if (!inode_manager.read_inode(current, inode)) {
        kfree(path);
        return "invalid inode";
      }

      char name[MAX_FILENAME_LENGTH];

      if (!directory_manager.find_name_by_inode(inode.parent_inode, current,
                                                name)) {
        kfree(path);
        return "path lookup failed";
      }

      StringUtils::strcpy(names[depth], name);

      depth++;

      current = inode.parent_inode;
    }

    // Build path backwards

    StringUtils::strcat(path, "/");

    for (int i = depth - 1; i >= 0; i--) {

      StringUtils::strcat(path, names[i]);

      if (i != 0)
        StringUtils::strcat(path, "/");
    }

    return path;
  }
};
