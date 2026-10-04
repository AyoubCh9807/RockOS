#pragma once

#include "../containers/string.hpp"
#include "../containers/vector.hpp"
#include "../shared/types.hpp"

struct BehemothEntry {
  u64 timestamp;
  String message;
};

/* BehemothDB is a fixed-capacity circular event log for kernel telemetry */

class BehemothDB {

private:
  size_t capacity;
  size_t head = 0;
  size_t tail = 0;
  size_t count = 0;
  Vector<BehemothEntry> buffer;

public:
  // Initializes the circular log buffer with a fixed maximum capacity.
  BehemothDB(size_t max_capacity = 128) : capacity(max_capacity) {
    buffer.resize(capacity);
  };

  // Cleans up resources allocated by the log buffer.
  ~BehemothDB();

  // Appends a new timestamped log entry, overwriting the oldest if full.
  void push(u64 timestamp, const String &message) {
    buffer[head] = {timestamp, message};
    head = (head + 1) % capacity;
    if (count < capacity)
      count++;
  };

  BehemothEntry read() {
    if (empty())
      return BehemothEntry{0, ""};
    BehemothEntry e = buffer[tail];
    tail = (tail + 1) % capacity;
    if (count > 0)
      count--;
    return e;
  }

  // Retrieves a log entry by its relative chronological index.
  bool get(size_t index, BehemothEntry &out_entry) const {
    if (index >= count)
      return false;

    out_entry = buffer[(index + tail) % capacity];
    return true;
  };

  // Clears all entries from the buffer without altering capacity.
  void clear() { buffer.clear(); };

  void fast_clear() { buffer.fast_clear(); }

  // Returns the current number of active log entries stored.
  size_t size() const { return count; };

  // Returns the maximum fixed capacity of the buffer.
  size_t capacity_size() const { return capacity; };

  // Returns true if the log buffer contains no entries.
  bool empty() const { return count == 0; };

  // Returns true if the log buffer has reached its maximum capacity.
  bool full() const { return count == capacity; };

  // Executes the provided callback function for every entry in chronological
  // order.
  template <typename Predicate> void for_each(Predicate pred) const {
    for (int i = 0; i < count; i++) {
      size_t idx = (tail + i) % capacity;
      pred(buffer[idx]);
    }
  };
};
