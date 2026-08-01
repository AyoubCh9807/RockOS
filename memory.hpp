#pragma once

typedef unsigned char u8;
typedef unsigned long long size_t;

struct Header {
  size_t size;
  bool is_free;
  Header *next;
  Header *prev;
};

inline u8 *heap_start = (u8 *)0x00400000;
inline u8 *heap_end = (u8 *)0x00800000;

// This function allocates memory

inline void init_heap() {
  Header *first_block = (Header *)heap_start;
  first_block->size = (heap_end - heap_start - sizeof(Header));
  first_block->is_free = true;
  first_block->prev = nullptr;
  first_block->next = nullptr;
};

inline void *kmalloc(size_t size) {
  if (size == 0)
    return 0;

  size_t total_needed = size + sizeof(Header);
  Header *current = (Header *)heap_start;

  while (current != nullptr) {
    if (current->is_free && current->size >= total_needed) {

      if (current->size > total_needed + sizeof(Header)) {
        Header *remaining = (Header *)((u8 *)current + sizeof(Header) + size);

        remaining->size = current->size - total_needed;
        remaining->is_free = true;
        remaining->next = current->next;
        remaining->prev = current;

        if (current->next) {
          current->next->prev = remaining;
        }
        current->next = remaining;

        current->size = size;
      }

      current->is_free = false;
      return (void *)((u8 *)current + sizeof(Header));
    }

    current = current->next;
  }

  return 0;
}

// This function frees memory
inline void *kfree(void *ptr) {
  if (!ptr)
    return 0;
  Header *header = (Header *)((u8 *)ptr - sizeof(Header));
  header->is_free = true;

  // We check the previous block is it exists and is free
  if (header->prev != nullptr && header->prev->is_free) {
    header->prev->size += header->size + sizeof(Header);

    // currently we are here: prev -> HERE -> next
    // we want (prev + HERE) -> next

    // We make next point back to (prev + HERE)
    if (header->next != nullptr)
      header->next->prev = header->prev;

    // We make (prev + HERE) point to next
    header->prev->next = header->next;

    header = header->prev;
  }

  // We check if the next block exists and is free
  if (header->next != nullptr && header->next->is_free) {

    header->size += header->next->size + sizeof(Header);

    if (header->next->next != nullptr) {
      header->next->next->prev = header;

      header->next = header->next->next;
    } else {
      header->next = nullptr;
    }
  }

  return 0;
}
