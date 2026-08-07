#pragma once
#include "string_utils.hpp"

namespace FSUtils {
inline static char *basename(char *path) {
  int last = -1;

  int len = StringUtils::strlen(path);

  for (int i = 0; i < len; i++) {
    if (path[i] == '/')
      last = i;
  }

  return &path[last + 1];
}
} // namespace Utils
