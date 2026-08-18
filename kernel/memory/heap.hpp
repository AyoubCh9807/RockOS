#pragma once
#include "../events/allocation_train.hpp"
#include "../shared/types.hpp"

struct Header {
  size_t size;
  bool is_free;
  Header *next;
  Header *prev;
};

class Heap {

private:
  u8 *heap_start = (u8 *)0x00400000;
  u8 *heap_end = (u8 *)0x00800000;
  size_t heap_used = 0;
  Events &allocation_events;

public:
  Heap(Events &e) : allocation_events(e) {};

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

    AllocationEvent ev;
    size_t total_needed = size + sizeof(Header);

    ev.size = total_needed;
    ev.type = ev.ALLOCATED;

    Header *current = (Header *)heap_start;

    while (current != nullptr) {
      if (current->is_free && current->size >= size) {

        if (current->size >= size + sizeof(Header) + 1) {
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
        heap_used += total_needed;
        allocation_events.push_event(ev);
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

    heap_used -= header->size + sizeof(Header);

    AllocationEvent ev;
    ev.size = header->size + sizeof(Header);
    ev.type = ev.FREED;

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

    allocation_events.push_event(ev);
    return 0;
  }

  size_t const get_used() const { return heap_used; }

  Events &get_event_stream() { return allocation_events; }

  void flood() {
    for (int i = 0; i < 1000; i++) {
      if (!kmalloc(4096)) {
        return;
      }
    }
  }
};

extern "C" void *memset(void *dest, int val, size_t num);
extern "C" void *memcpy(void *dest, const void *src, size_t size);

inline Events events;
inline Heap heap(events);

inline void *kmalloc(size_t size) { return heap.kmalloc(size); }

inline void kfree(void *ptr) { heap.kfree(ptr); }

inline const size_t get_used() { return heap.get_used(); }
