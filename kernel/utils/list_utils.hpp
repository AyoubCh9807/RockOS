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

// Helper to recursively select a list by index and return a pointer to it
template <size_t N1, size_t... RestNs>
static const char *const *pick_list(size_t index,
                                    const char *const (&list1)[N1],
                                    const char *const (&...rest)[RestNs]) {
  if (index == 0) {
    return list1;
  }
  return pick_list(index - 1, rest...);
}

/**
 * This function accepts a variable number of string list references, picks
 * one of the lists at random, and returns a pointer to it (decayed to const
 * char *const *).
 */
template <size_t... Ns>
static const char *const *get_random_list(const char *const (&...lists)[Ns]) {
  constexpr size_t num_lists = sizeof...(Ns);

  static_assert(num_lists > 0, "At least one list must be provided.");

  size_t index = Random::next() % num_lists;
  return pick_list(index, lists...);
}

} // namespace ListUtils
