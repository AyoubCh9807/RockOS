#pragma once

#include "../containers/string.hpp"
#include "../containers/vector.hpp"
#include "../shared/types.hpp"

/* XanaxDB is a dynamic KV store optimized for low-latency kernel operations */

template <typename K, typename V> class XanaxDB {

private:
  size_t count = 0;
  Vector<K> keys = Vector<K>();
  Vector<V> values = Vector<V>();

public:
  // Initializes the database with default capacity.
  XanaxDB() {};

  // Cleans up database resources and internal vectors.
  ~XanaxDB() = default; 

  // Inserts or updates a key value pair.
  void set(const K &key, const V &val) {
    int idx = keys.find(key);
    if (idx == -1) {
      keys.push_back(key);
      values.push_back(val);
    } else {
      values[idx] = val;
    }
  };

  // Retrieves the value for a key or returns a default instance if missing.
  V get(const K &key) const {
    size_t s = keys.size();
    for (size_t i = 0; i < s; i++) {
      if (key == keys[i])
        return values[i];
    }
    return values[0];
  };

  // Safely looks up a key and populates out_val, returning true if found.
  bool get(const K &key, V &out_val) const {
    size_t s = keys.size();
    for (size_t i = 0; i < s; i++) {
      if (key == keys[i]) {
        out_val = values[i];
        return true;
      }
    }
    return false;
  };

  // Removes a key and its corresponding value from the database.
  bool remove(const K &key) {
    V val = get(key);
    int idx = keys.find(key);
    if (idx == -1)
      return false;
    keys.erase(idx);
    values.erase(idx);
    return true;
  };

  // Resets the database, clearing all keys and values.
  void clear() {
    keys.clear();
    values.clear();
  };

  void fast_clear() {
    keys.fast_clear();
    values.fast_clear();
  };

  // Returns true if the key exists in the database.
  bool contains(const K &key) const { return keys.find(key) != -1; };

  // Returns the total number of key value pairs currently stored.
  size_t size() const { return keys.size(); };

  // Returns true if the database contains no entries.
  bool empty() const { return keys.empty() && values.empty(); };

  // Locates the internal index of a key, returning negative one if not found.
  int find_index(const K &key) const { return keys.find(key); };

  // Trims internal storage capacity to match the current item count.
  void shrink_to_fit() {
    keys.shrink_to_fit();
    values.shrink_to_fit();
  };

  // Gathers all keys matching the provided prefix string.
  size_t get_keys_by_prefix(const K &prefix, Vector<K> &out_keys) const;

  // Executes the provided callback function on the key then on the value.
  template <typename Predicate> void for_each(Predicate pred) const {
    size_t len = keys.size();
    for (int i = 0; i < len; i++) {
      pred(keys[i]);
      pred(values[i]);
    }
  };

  // Executes the provided callback function on the key and the value at the same time.
  template <typename Predicate> void for_each_pair(Predicate pred) const {
    size_t len = keys.size();
    for (int i = 0; i < len; i++) {
      pred(keys[i], values[i]);
    }
  };

  // Executes the provided callback function on the key.
  template <typename Predicate> void for_each_key(Predicate pred) const {
    size_t len = keys.size();
    for (int i = 0; i < len; i++) {
      pred(keys[i]);
    }
  };

  // Executes the provided callback function on the value.
  template <typename Predicate> void for_each_val(Predicate pred) const {
    size_t len = keys.size();
    for (int i = 0; i < len; i++) {
      pred(values[i]);
    }
  };
};
