#pragma once

#include "../containers/vector.hpp"
#include "../shared/types.hpp"

/* AnomalyDB is a high-performance probabilistic gatekeeper (Bloom filter)
 * designed to intercept non-existent lookups and shield slow primary storage.
 */

class AnomalyDB {

private:
  size_t bit_capacity;   // Total number of bits (m)
  size_t hash_count;     // Number of hash functions (k)
  size_t item_count = 0; // Number of items inserted (n)
  Vector<u64> bitfield;  // Raw bit array stored in 64-bit blocks

  // Simple, fast non-cryptographic hash combiner 
  u64 hash(const u8 *data, size_t len, u64 seed) const {
    u64 h = 14695981039346656037ULL ^ seed;
    for (size_t i = 0; i < len; ++i) {
      h ^= data[i];
      h *= 1099511628211ULL;
    }
    return h;
  }

public:
  // Initializes the filter with a fixed bit capacity and number of hash
  // functions.
  AnomalyDB(size_t capacity_bits = 1024, size_t num_hashes = 3)
      : bit_capacity(capacity_bits), hash_count(num_hashes) {
    size_t u64_chunks = (bit_capacity + 63) / 64;
    bitfield.resize(u64_chunks, 0);
  }

  // Cleans up resources.
  ~AnomalyDB() = default;

  // Inserts a raw byte key into the filter.
  void insert(const u8 *data, size_t len) {
    for (size_t i = 0; i < hash_count; ++i) {
      u64 hash_val = hash(data, len, i + 1);
      size_t bit_index = hash_val % bit_capacity;

      size_t chunk_idx = bit_index / 64;
      size_t bit_offset = bit_index % 64;

      bitfield[chunk_idx] |= (1ULL << bit_offset);
    }
    item_count++;
  }

  // Tests membership. Returns false if definitely absent, true if probably
  // present.
  bool might_contain(const u8 *data, size_t len) const {
    for (size_t i = 0; i < hash_count; ++i) {
      u64 hash_val = hash(data, len, i + 1);
      size_t bit_index = hash_val % bit_capacity;

      size_t chunk_idx = bit_index / 64;
      size_t bit_offset = bit_index % 64;

      if ((bitfield[chunk_idx] & (1ULL << bit_offset)) == 0) {
        return false; // Zero false negatives so like if any bit is 0, it's 100% not
                      // here
      }
    }
    return true; // Probably present (subject to false positive rate p)
  }

  // Wipes all bits clean without altering overall capacity.
  void clear() {
    for (size_t i = 0; i < bitfield.size(); ++i) {
      bitfield[i] = 0;
    }
    item_count = 0;
  }

  void fast_clear() {
    bitfield.fast_clear();
    item_count = 0;
  }

  // Returns the number of items recorded as inserted.
  size_t size() const { return item_count; }

  // Returns the total bit capacity of the filter.
  size_t capacity() const { return bit_capacity; }
};
