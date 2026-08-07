#pragma once

#include "../shared/types.hpp"
#include "../utils/string_utils.hpp"
#include "../memory/memory.hpp"
#include "directory_manager.hpp"
#include "inode_manager.hpp"
#include "layout.hpp"

class PathResolver {
private:
	InodeManager &inode_manager;
	DirectoryManager &directory_manager;

	// small local bounded append (so we don't rely on your old unbounded append)
	static void append_bounded(char *dst, u32 cap, const char *src) {
		if (!dst || cap == 0) return;
		if (!src) src = "";
		u32 len = (u32)StringUtils::strlen(dst);
		if (len >= cap) { dst[cap - 1] = '\0'; return; }
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

	u32 resolve_path(char *path) {
		if (!path) return INVALID_INODE;
		if (path[0] == '\0') return ROOT_INODE;

		char *args[16];
		int count = StringUtils::split_by(path, '/', args, 16);

		u32 current_inode_number = ROOT_INODE;

		DirectoryEntry out{};
		for (int i = 0; i < count; i++) {
			if (!args[i] || args[i][0] == '\0') continue;

			bool found = directory_manager.find_entry(current_inode_number, args[i], out);
			if (!found) return INVALID_INODE;

			Inode inode{};
			if (!inode_manager.read_inode(out.inode_number, inode)) return INVALID_INODE;

			bool is_last = (i == count - 1);
			if (!is_last && !inode.is_directory) return INVALID_INODE;

			current_inode_number = out.inode_number;
		}

		return current_inode_number;
	}

	u32 resolve_parent(char *path) {
		if (!path) return INVALID_INODE;

		// "/" -> parent is root
		if (path[0] == '/' && path[1] == '\0') return ROOT_INODE;

		int path_len = StringUtils::strlen(path);
		if (path_len == 0) return INVALID_INODE;

		int index = -1;
		for (int i = path_len - 1; i >= 0; i--) {
			if (path[i] == '/') { index = i; break; }
		}

		if (index == 0) return ROOT_INODE;     // "/x" parent is root
		if (index == -1) return ROOT_INODE;    // "x" parent treated as root for now

		path[index] = '\0';
		return resolve_path(path);
	}

	const char *get_path(u32 inode_number) {
		if (inode_number == INVALID_INODE) return "invalid inode";

		// 256 is fine for now given your depth cap (16) and name cap (32)
		char *path = (char *)kmalloc(256);
		if (!path) return "out of memory";
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
			if (!directory_manager.find_name_by_inode(inode.parent_inode, current, name)) {
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
			if (i != 0) append_bounded(path, 256, "/");
		}

		return path;
	}
};
