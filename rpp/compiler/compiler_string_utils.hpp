#include "stdarg.h"

#include <string>
#include <cstdio>
#include <bit>
#include <cstdint>

class StringUtils {
public:
  static std::string format(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    va_list copy;
    va_copy(copy, args);

    int size = std::vsnprintf(nullptr, 0, fmt, copy);

    va_end(copy);

    if (size < 0) {
      va_end(args);
      return {};
    }

    std::string result(size, '\0');

    std::vsnprintf(result.data(), size + 1, fmt, args);

    va_end(args);

    return result;
  }
};
