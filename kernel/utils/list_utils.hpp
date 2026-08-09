#pragma once

#include "../random/random.hpp"

namespace ListUtils {
// Base case for recursion
template <size_t N>
const char *pick_from_array(size_t index, const char *const (&list)[N]) {
  return list[index];
}

// Recursive variadic helper to navigate through the list arguments
template <size_t N1, size_t... RestNs>
static const char *pick_from_array(size_t index, const char *const (&list1)[N1],
                            const char *const (&...rest)[RestNs]) {
  if (index < N1) {
    return list1[index];
  }
  return pick_from_array(index - N1, rest...);
}

/**
 * This function accepts a variable number of string list references, computes
 * the total size at compile time using fold expressions, and returns a random
 * string.
 */
template <size_t... Ns>
static const char *get_random_string(const char *const (&...lists)[Ns]) {
  // Calculate total number of strings across all lists at compile time
  constexpr size_t total_n = (Ns + ...);

  static_assert(total_n > 0, "The provided string lists cannot be empty.");

  size_t index = Random::next() % total_n;
  return pick_from_array(index, lists...);
}
} // namespace ListUtils
