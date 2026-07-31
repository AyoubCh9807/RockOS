#pragma once

typedef unsigned char u8;
typedef unsigned long long size_t;

inline u8 *heap_start = (u8 *)0x00400000;
inline u8 *heap_end = (u8 *)0x00800000;
inline u8 *heap_current = (u8 *)0x00400000;

// This function allocates memory
inline void *kmalloc(size_t size) {
  if (heap_current + size > heap_end) {
    return 0; // Out of memory
  }
  u8 *ptr = heap_current;
  heap_current += size;
  return (void *)ptr;
}

// This function frees memory
inline void *kfree(u8 *ptr) {
  (void)ptr;
  // Nothin for now
  return 0;
}


