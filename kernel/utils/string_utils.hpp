#pragma once

#include "../memory/heap.hpp"
#include <stdarg.h>

constexpr int DEFAULT_BUFFER_ALLOCATION_SIZE = 16;
constexpr int DEFAULT_BUFFER_INCREMENT = 16;

constexpr int NUMERIC_DIGITS_START = 48;
constexpr int NUMERIC_DIGITS_END = 57;

constexpr int UPPERCASE_START = 65;
constexpr int UPPERCASE_END = 90;
constexpr int LOWERCASE_START = 97;
constexpr int LOWERCASE_END = 122;

// Forward declaration — no include needed
class String;

namespace StringUtils {

inline int strcmp(const char *s1, const char *s2) {
  if (!s1 && !s2)
    return 0;
  if (!s1)
    return -1;
  if (!s2)
    return 1;

  int i = 0;
  while (s1[i] != '\0' && s2[i] != '\0') {
    if (s1[i] != s2[i])
      return (unsigned char)s1[i] - (unsigned char)s2[i];
    i++;
  }
  return (unsigned char)s1[i] - (unsigned char)s2[i];
}

inline int split_by(char *str, const char separator, char **args,
                    int max_args) {
  if (!str || !args || max_args <= 0)
    return 0;

  int arg_count = 0;
  bool in_word = false;

  for (int i = 0; str[i] != '\0'; i++) {
    if (str[i] == separator) {
      str[i] = '\0';
      in_word = false;
    } else if (str[i] != ' ' && str[i] != '\n' && str[i] != '\r' &&
               str[i] != '\t') {
      if (!in_word) {
        if (arg_count < max_args)
          args[arg_count++] = &str[i];
        in_word = true;
      }
    }
  }
  return arg_count;
}

inline int strlen(const char *str) {
  if (!str)
    return 0;
  int i = 0;
  while (str[i] != '\0')
    i++;
  return i;
}

inline char *strcat_alloc(const char *str1, const char *str2) {
  if (!str1)
    str1 = "";
  if (!str2)
    str2 = "";

  int len1 = strlen(str1);
  int len2 = strlen(str2);

  char *dest = (char *)kmalloc((size_t)len1 + (size_t)len2 + 1);
  if (!dest)
    return nullptr;

  int idx = 0;
  for (int i = 0; i < len1; i++)
    dest[idx++] = str1[i];
  for (int i = 0; i < len2; i++)
    dest[idx++] = str2[i];
  dest[idx] = '\0';
  return dest;
}

inline char *iota(int val) {
  long long v = (long long)val;
  bool neg = false;

  if (v < 0) {
    neg = true;
    v = -v;
  }

  int len = 0;
  long long tmp = v;
  do {
    tmp /= 10;
    len++;
  } while (tmp != 0);

  if (neg)
    len++;

  char *str = (char *)kmalloc((size_t)len + 1);
  if (!str)
    return nullptr;

  str[len] = '\0';
  int i = len - 1;

  do {
    str[i--] = '0' + (v % 10);
    v /= 10;
  } while (v != 0);

  if (neg)
    str[0] = '-';

  return str;
}

inline void append(char *dest, size_t dest_capacity, const char *src) {
  if (!dest || dest_capacity == 0)
    return;
  if (!src)
    src = "";

  size_t dest_len = (size_t)strlen(dest);
  if (dest_len >= dest_capacity) {
    dest[dest_capacity - 1] = '\0';
    return;
  }

  size_t i = 0;
  while (src[i] != '\0' && (dest_len + i + 1) < dest_capacity) {
    dest[dest_len + i] = src[i];
    i++;
  }
  dest[dest_len + i] = '\0';
}

inline void append(char *dest, const char *src) {
  if (!dest || !src)
    return;
  int dest_len = strlen(dest);
  int i = 0;
  while (src[i] != '\0') {
    dest[dest_len + i] = src[i];
    i++;
  }
  dest[dest_len + i] = '\0';
}

inline char *grow_buffer(char *buf, size_t &capacity, int used, size_t needed) {
  if (needed <= capacity)
    return buf;

  size_t new_capacity = capacity;
  while (new_capacity < needed)
    new_capacity += DEFAULT_BUFFER_INCREMENT;

  char *new_buf = (char *)kmalloc(new_capacity);
  if (!new_buf)
    return nullptr;

  for (int i = 0; i < used; i++)
    new_buf[i] = buf[i];

  if (buf)
    kfree(buf);
  capacity = new_capacity;
  return new_buf;
}

inline void strcpy(char *dest, const char *src) {
  if (!dest || !src)
    return;
  int i = 0;
  while (src[i] != '\0') {
    dest[i] = src[i];
    i++;
  }
  dest[i] = '\0';
}

inline int find(char *str, char c) {
  if (!str)
    return -1;
  for (int i = 0; str[i] != '\0'; i++)
    if (str[i] == c)
      return i;
  return -1;
}

inline char *slice(char *str, int start, int end) {
  if (!str)
    return nullptr;
  int length = strlen(str);
  if (start >= end)
    return nullptr;
  if (start < 0 || end > length)
    return nullptr;

  int size = end - start;
  char *res = (char *)kmalloc((size_t)size + 1);
  if (!res)
    return nullptr;

  for (int i = 0; i < size; i++)
    res[i] = str[start + i];
  res[size] = '\0';
  return res;
}

inline bool contains(const char *str, char c) {
  if (!str)
    return false;
  for (int i = 0; str[i] != '\0'; i++)
    if (str[i] == c)
      return true;
  return false;
}

inline void print_number_into(char *buf, int cap, int n) {
  if (!buf || cap <= 0)
    return;

  int i = cap - 1;
  buf[i--] = '\0';
  bool neg = n < 0;
  unsigned int v = neg ? (unsigned int)(-(long long)n) : (unsigned int)n;
  if (v == 0)
    buf[i--] = '0';
  while (v != 0 && i >= 0) {
    buf[i--] = '0' + (v % 10);
    v /= 10;
  }
  if (neg && i >= 0)
    buf[i--] = '-';

  int j = 0;
  for (int k = i + 1; buf[k] != '\0'; k++, j++)
    buf[j] = buf[k];
  buf[j] = '\0';
}

inline void substr(char *str, int start, int end) {
  if (start >= end) {
    str[0] = '\0';
    return;
  }

  int i = 0;
  while (start + i < end) {
    str[i] = str[start + i];
    i++;
  }
  str[i] = '\0';
}

inline bool is_numeric(const char *str) {
  if (str == nullptr || str[0] == '\0')
    return false;

  int i = 0;
  while (str[i] != '\0') {
    int c = (int)str[i];
    if (c < NUMERIC_DIGITS_START || c > NUMERIC_DIGITS_END)
      return false;
    i++;
  }
  return true;
}

inline bool is_alpha(const char *str) {
  if (str == nullptr || str[0] == '\0')
    return false;

  int i = 0;
  while (str[i] != '\0') {
    int c = (int)str[i];
    bool is_upper = (c >= UPPERCASE_START && c <= UPPERCASE_END);
    bool is_lower = (c >= LOWERCASE_START && c <= LOWERCASE_END);

    if (!is_upper && !is_lower)
      return false;
    i++;
  }
  return true;
}

inline int vsnprintf(char *buf, size_t max_len, const char *fmt, va_list args) {
  if (!buf || max_len == 0)
    return 0;
  if (!fmt) {
    buf[0] = '\0';
    return 0;
  }

  size_t i = 0;
  for (const char *p = fmt; *p && i < max_len - 1; p++) {
    if (*p != '%') {
      buf[i++] = *p;
      continue;
    }

    p++;
    if (!*p)
      break;

    switch (*p) {
    case '%':
      buf[i++] = '%';
      break;

    case 'c': {
      char c = (char)va_arg(args, int);
      if (i < max_len - 1)
        buf[i++] = c;
      break;
    }

    case 's': {
      const char *s = va_arg(args, const char *);
      if (!s)
        s = "(null)";
      while (*s && i < max_len - 1)
        buf[i++] = *s++;
      break;
    }

    case 'd': {
      int n = va_arg(args, int);
      char num_buf[16];
      bool neg = n < 0;
      unsigned int un = neg ? -n : n;
      int num_i = 0;
      do {
        num_buf[num_i++] = '0' + (un % 10);
        un /= 10;
      } while (un);
      if (neg && i < max_len - 1)
        buf[i++] = '-';
      while (num_i > 0 && i < max_len - 1)
        buf[i++] = num_buf[--num_i];
      break;
    }

    case 'u': {
      unsigned int n = va_arg(args, unsigned int);
      char num_buf[16];
      int num_i = 0;
      do {
        num_buf[num_i++] = '0' + (n % 10);
        n /= 10;
      } while (n);
      while (num_i > 0 && i < max_len - 1)
        buf[i++] = num_buf[--num_i];
      break;
    }

    case 'x':
    case 'X': {
      unsigned int n = va_arg(args, unsigned int);
      char num_buf[8];
      int num_i = 0;
      const char *digits =
          (*p == 'x') ? "0123456789abcdef" : "0123456789ABCDEF";
      do {
        num_buf[num_i++] = digits[n & 0xF];
        n >>= 4;
      } while (n);
      while (num_i > 0 && i < max_len - 1)
        buf[i++] = num_buf[--num_i];
      break;
    }

    case 'p': {
      void *ptr = va_arg(args, void *);
      unsigned long addr = (unsigned long)ptr;
      if (i < max_len - 1)
        buf[i++] = '0';
      if (i < max_len - 1)
        buf[i++] = 'x';
      char num_buf[16];
      int num_i = 0;
      do {
        num_buf[num_i++] = "0123456789abcdef"[addr & 0xF];
        addr >>= 4;
      } while (addr);
      while (num_i > 0 && i < max_len - 1)
        buf[i++] = num_buf[--num_i];
      break;
    }

    default:
      if (i < max_len - 1)
        buf[i++] = *p;
      break;
    }
  }

  buf[i] = '\0';
  return (int)i;
}

inline int snprintf(char *buf, size_t max_len, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  int ret = vsnprintf(buf, max_len, fmt, args);
  va_end(args);
  return ret;
}

// Declared here but defined in string.hpp after String class is complete
String format(const char *fmt, ...);

static int to_int(const char *str) {
  if (!str)
    return 0;

  int result = 0;
  int sign = 1;

  if (*str == '-') {
    sign = -1;
    str++;
  }

  while (*str >= '0' && *str <= '9') {
    result = result * 10 + (*str - '0');
    str++;
  }

  return result * sign;
}

void reverse(const char *str, char *buf) {
  int length = strlen(str);

  for (int i = 0; i < length; i++)
    buf[i] = str[length - 1 - i];

  buf[length] = '\0';
}

} // namespace StringUtils
