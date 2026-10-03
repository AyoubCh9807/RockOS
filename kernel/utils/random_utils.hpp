#pragma once

#include "../random/random.hpp"

namespace RandomUtils {
inline int random_int() { return Random::next(); }

inline int random_int_neg() {
  int val = Random::next();
  return val * ((val % 2) * -1);
}

inline bool random_bool() { return Random::next() % 2; }

inline float random_float() {
  float v1 = (float)Random::next() + 1;
  float v2 = (float)Random::next() + 1;
  return v1 > v2 ? v2 / v1 : v1 / v2;
}

inline float random_float_neg() {
  float val = random_float();
  return val * (((int)val % 2) * -1);
}

inline double random_double() {
  double v1 = (double)Random::next() + 1;
  double v2 = (double)Random::next() + 1;
  return v1 > v2 ? v2 / v1 : v1 / v2;
}

inline double random_double_neg() {
  double val = random_double();
  return val * (((int)val % 2) * -1);
}

inline int random_from_range(int min, int max) {
  return (Random::next() % (max - min + 1)) + min;
}
} // namespace RandomUtils
