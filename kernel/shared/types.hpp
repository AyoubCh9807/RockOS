#pragma once
#include "../../kernel/shared/types.hpp"

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef __SIZE_TYPE__ size_t;

template <typename To, typename From> To bit_cast(const From &value) {
  static_assert(sizeof(To) == sizeof(From));

  To result;

  const u8 *src = reinterpret_cast<const u8 *>(&value);
  u8 *dst = reinterpret_cast<u8 *>(&result);

  for (size_t i = 0; i < sizeof(To); ++i)
    dst[i] = src[i];

  return result;
}
