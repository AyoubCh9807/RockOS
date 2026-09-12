#pragma once
#include "../shared/types.hpp"

namespace MathUtils {
static constexpr u8 bcd_to_binary(u8 value) {
  return (value & 0x0F) + ((value >> 4) * 10);
}
template <typename T> T expr_result(T x, T y, char op) {
  switch (op) {
  case '+':
    return x + y;

  case '-':
    return x - y;

  case '*':
    return x * y;

  case '/':
    return x / y;

  default:
    return 0;
  }
}

static void int_to_hex(u32 value, char *buf) {
  const char *hex = "0123456789ABCDEF";

  buf[0] = '0';
  buf[1] = 'x';

  for (int i = 0; i < 8; i++) {
    int shift = (7 - i) * 4;
    buf[2 + i] = hex[(value >> shift) & 0xF];
  }

  buf[10] = '\0';
}

template <typename T> T max(T a, T b) { return a > b ? a : b; }

template <typename T> T min(T a, T b) { return a < b ? a : b; }

template <typename T> T clamp(T min, T mid, T max) {
  T res;
  res = (min > mid ? min : mid);
  res = (res > max ? max : res);
  return res;
}

inline constexpr double PI = 3.14159265358979323846;
inline constexpr double TwoPi = 2.0 * PI;

template <typename T> constexpr T normalize_radians(T radians) {
  while (radians > static_cast<T>(PI))
    radians -= static_cast<T>(TwoPi);
  while (radians < static_cast<T>(-PI))
    radians += static_cast<T>(TwoPi);
  return radians;
}

template <typename T> T sin(T x) {
  x = normalize_radians(x);
  T term = x;
  T sum = x;
  T x_squared = x * x;

  // 10 terms balance high accuracy and performance
  for (int i = 1; i <= 10; ++i) {
    term *= -x_squared / (static_cast<T>(2 * i) * static_cast<T>(2 * i + 1));
    sum += term;
  }
  return sum;
}

template <typename T> T cos(T x) {
  x = normalize_radians(x);
  T term = static_cast<T>(1.0);
  T sum = static_cast<T>(1.0);
  T x_squared = x * x;

  for (int i = 1; i <= 10; ++i) {
    term *= -x_squared / (static_cast<T>(2 * i - 1) * static_cast<T>(2 * i));
    sum += term;
  }
  return sum;
}

template <typename T> T exp(T x) {
  T sum = static_cast<T>(1.0);
  T term = static_cast<T>(1.0);
  for (int i = 1; i <= 20; ++i) {
    term *= x / static_cast<T>(i);
    sum += term;
  }
  return sum;
}

template <typename T> T ln(T x) {
  if (x <= static_cast<T>(0))
    return static_cast<T>(0);
  T y = (x - static_cast<T>(1.0)) / (x + static_cast<T>(1.0));
  T y2 = y * y;
  T term = y;
  T sum = y;
  for (int i = 1; i <= 15; ++i) {
    term *= y2;
    sum += term / static_cast<T>(2 * i + 1);
  }
  return static_cast<T>(2.0) * sum;
}

template <typename T> T pow(T base, T exponent) {
  if (base == static_cast<T>(0))
    return static_cast<T>(0);
  if (exponent == static_cast<T>(0))
    return static_cast<T>(1.0);

  // Handle negative bases with integer exponents cleanly via binary
  // exponentiation
  if (base < static_cast<T>(0)) {
    long long intExp = static_cast<long long>(exponent);
    if (static_cast<T>(intExp) == exponent) {
      T result = static_cast<T>(1.0);
      T b = base;
      long long e = intExp;
      if (e < 0) {
        b = static_cast<T>(1.0) / b;
        e = -e;
      }
      while (e > 0) {
        if (e & 1)
          result *= b;
        b *= b;
        e >>= 1;
      }
      return result;
    }
    return static_cast<T>(
        0); // Domain error for non-integer exponent with negative base
  }

  // General case: base^exponent = exp(exponent * ln(base))
  return exp(exponent * ln(base));
}

template <typename T> T sqrt(T x) {
  if (x < static_cast<T>(0))
    return static_cast<T>(0); // Domain error handling

  // Initial guess
  T guess = x / static_cast<T>(2.0);
  if (guess == static_cast<T>(0))
    guess = static_cast<T>(1.0);

  // 10-15 iterations typically achieve full double-precision accuracy
  for (int i = 0; i < 15; ++i) {
    guess = static_cast<T>(0.5) * (guess + x / guess);
  }
  return guess;
}

} // namespace MathUtils
