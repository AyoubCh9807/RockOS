#pragma once

#include "string_utils.hpp"

namespace FSUtils {

// Returns pointer into `path` (no allocation). `path` must remain valid.
// Works with or without '/' present.
inline static char *basename(char *path) {
	if (!path) return path;

	int last = -1;
	int len = StringUtils::strlen(path);

	for (int i = 0; i < len; i++) {
		if (path[i] == '/') last = i;
	}
	return &path[last + 1];
}

inline static const char *basename(const char *path) {
	return basename(const_cast<char *>(path));
}

} // namespace FSUtils
