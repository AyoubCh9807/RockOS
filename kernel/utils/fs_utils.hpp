#pragma once

#include "string_utils.hpp"

namespace FSUtils {

// Returns pointer into `path` (no allocation). `path` must remain valid.
// Works with or without '/' present.
inline static char *basename(char *path) {
  if (!path)
    return path;

  int last = -1;
  int len = StringUtils::strlen(path);

  for (int i = 0; i < len; i++) {
    if (path[i] == '/')
      last = i;
  }
  return &path[last + 1];
}

inline static const char *basename(const char *path) {
  return basename(const_cast<char *>(path));
}

inline static char *dirname(char *path, char *buf) {
  if (!path || !buf)
    return nullptr;

  int last = -1;
  int len = StringUtils::strlen(path);

  for (int i = 0; i < len; i++) {
    if (path[i] == '/')
      last = i;
  }

  if (last == -1) {
    buf[0] = '.';
    buf[1] = '\0';
    return buf;
  }

  if (last == 0) {
    buf[0] = '/';
    buf[1] = '\0';
    return buf;
  }

  for (int i = 0; i < last; i++)
    buf[i] = path[i];

  buf[last] = '\0';

  return buf;
}

} // namespace FSUtils
