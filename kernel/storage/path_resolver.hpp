#pragma once

#include "../memory/heap.hpp"
#include "../shared/types.hpp"
#include "../utils/string_utils.hpp"
#include "directory_manager.hpp"
#include "inode_manager.hpp"
#include "layout.hpp"

class PathResolver {
private:
  InodeManager &inode_manager;
  DirectoryManager &directory_manager;

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

public:
  PathResolver(InodeManager &inode_manager, DirectoryManager &directory_manager)
      : inode_manager(inode_manager), directory_manager(directory_manager) {}

  // base_dir is where relative paths (no leading '/') start resolving from.
  // A leading '/' always means "start at root", regardless of base_dir.
  u32 resolve_path(char *path, u32 base_dir = ROOT_INODE) {
    if (!path)
      return INVALID_INODE;
    if (path[0] == '\0')
      return base_dir;

    u32 current_inode_number = (path[0] == '/') ? ROOT_INODE : base_dir;

    char *args[16];
    int count = StringUtils::split_by(path, '/', args, 16);

    DirectoryEntry out{};
    for (int i = 0; i < count; i++) {
      if (!args[i] || args[i][0] == '\0')
        continue;

      bool found =
          directory_manager.find_entry(current_inode_number, args[i], out);
      if (!found)
        return INVALID_INODE;

      Inode inode{};
      if (!inode_manager.read_inode(out.inode_number, inode))
        return INVALID_INODE;

      bool is_last = (i == count - 1);
      if (!is_last && !inode.is_directory)
        return INVALID_INODE;

      current_inode_number = out.inode_number;
    }

    return current_inode_number;
  }

  u32 resolve_parent(char *path, u32 base_dir = ROOT_INODE) {
    if (!path)
      return INVALID_INODE;

    bool absolute = (path[0] == '/');

    // "/" -> parent is root
    if (absolute && path[1] == '\0')
      return ROOT_INODE;

    int path_len = StringUtils::strlen(path);
    if (path_len == 0)
      return INVALID_INODE;

    int index = -1;
    for (int i = path_len - 1; i >= 0; i--) {
      if (path[i] == '/') {
        index = i;
        break;
      }
    }

    if (absolute && index == 0)
      return ROOT_INODE; // "/x" parent is root

    if (index == -1)
      return base_dir; // "x" (no slash) -> parent is the current directory

    path[index] = '\0';
    // If what remains still starts with '/', resolve_path will correctly
    // treat it as absolute regardless of base_dir.
    return resolve_path(path, base_dir);
  }

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

      Inode inode{};
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

    append_bounded(path, 256, "/");
    for (int i = depth - 1; i >= 0; i--) {
      append_bounded(path, 256, names[i]);
      if (i != 0)
        append_bounded(path, 256, "/");
    }

    return path;
  }
};
