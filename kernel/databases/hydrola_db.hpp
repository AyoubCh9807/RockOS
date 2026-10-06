#pragma once

#include "../containers/vector.hpp"
#include "../shared/types.hpp"

/* HydrolaDB is a template-based multi-index kernel storage engine.
 * It manages raw data payloads in a single body with multi-view lookup.
 */

template <typename T> class HydrolaDB {
private:
  Vector<T> body;

public:
  // Default constructor
  HydrolaDB() = default;

  // Default destructor
  ~HydrolaDB() = default;

  // Appends a new payload to the primary body and returns its storage index
  size_t insert(const T &payload) {
    body.push_back(payload);
    return body.size() - 1;
  };

  // Retrieves a mutable pointer to the payload by its body index
  T *get(size_t index) { return &body[index]; };

  // Retrieves an immutable pointer to the payload by its body index
  const T *get(size_t index) const { return &body[index]; };

  // Queries payloads using a predicate function, populating output indices
  template <typename Predicate>
  void query(Predicate pred, Vector<size_t> &out_indices) const {
    for (size_t i = 0; i < body.size(); i++) {
      if (pred(body[i])) {
        out_indices.push_back(i);
      }
    }
  };

  // Wipes all payloads from primary storage
  void clear() { body.clear(); };

  // Fast wipes storage without deallocating backing memory
  void fast_clear() { body.fast_clear(); };

  // Returns the total number of items stored
  size_t size() const { return body.size(); };

  // Checks if the storage body is empty
  bool empty() const { return body.empty(); };
};
