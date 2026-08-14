#include "../memory/memory.hpp"
#include "../shared/types.hpp"

extern "C" void *memset(void *dest, int value, size_t count) {
  u8 *ptr = (u8 *)dest;

  for (size_t i = 0; i < count; i++)
    ptr[i] = (u8)value;

  return dest;
}

extern "C" void *memcpy(void *dest, const void *src, size_t count) {
  u8 *d = (u8 *)dest;
  const u8 *s = (const u8 *)src;

  for (size_t i = 0; i < count; i++)
    d[i] = s[i];

  return dest;
}

void operator delete(void *ptr) noexcept {
  if (ptr)
    kfree(ptr);
}

void operator delete[](void *ptr) noexcept {
  if (ptr)
    kfree(ptr);
}

void operator delete(void *ptr, size_t) noexcept {
  if (ptr)
    kfree(ptr);
}

void operator delete[](void *ptr, size_t) noexcept {
  if (ptr)
    kfree(ptr);
}

void *operator new(size_t size) { return kmalloc(size); }

void *operator new[](size_t size) { return kmalloc(size); }
