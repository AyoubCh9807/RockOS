#pragma once

#include "../memory/heap.hpp"
#include "../shared/types.hpp"
#include "../utils/string_utils.hpp"

constexpr int DEFAULT_STRING_INCREMENT = 16;

class String {
  // Allow StringUtils::format to use private members for efficiency
  friend String StringUtils::format(const char *fmt, ...);

private:
  size_t size_;
  size_t capacity_;
  char *data_;

  static inline char err_char_ = '\0';

  void ensure_capacity(size_t needed) {
    if (needed <= capacity_)
      return;

    size_t new_cap = capacity_;
    if (new_cap == 0)
      new_cap = DEFAULT_STRING_INCREMENT;
    while (new_cap < needed)
      new_cap += DEFAULT_STRING_INCREMENT;

    char *new_data = (char *)kmalloc(new_cap);
    if (!new_data)
      return;

    for (size_t i = 0; i < size_; i++)
      new_data[i] = data_[i];
    new_data[size_] = '\0';

    if (data_)
      kfree(data_);
    data_ = new_data;
    capacity_ = new_cap;
  }

public:
  String() : size_(0), capacity_(DEFAULT_STRING_INCREMENT), data_(nullptr) {
    data_ = (char *)kmalloc(capacity_);
    if (data_)
      data_[0] = '\0';
    else
      capacity_ = 0;
  }

  String(const char *cstr) : size_(0), capacity_(0), data_(nullptr) {
    if (!cstr)
      cstr = "";
    size_ = (size_t)StringUtils::strlen(cstr);
    capacity_ = size_ + 1 + DEFAULT_STRING_INCREMENT;

    data_ = (char *)kmalloc(capacity_);
    if (!data_) {
      size_ = 0;
      capacity_ = 0;
      return;
    }

    for (size_t i = 0; i < size_; i++)
      data_[i] = cstr[i];
    data_[size_] = '\0';
  }

  String(const String &other) : size_(0), capacity_(0), data_(nullptr) {
    size_ = other.size_;
    capacity_ = other.capacity_;
    if (capacity_ == 0)
      return;

    data_ = (char *)kmalloc(capacity_);
    if (!data_) {
      size_ = 0;
      capacity_ = 0;
      return;
    }

    for (size_t i = 0; i < size_; i++)
      data_[i] = other.data_[i];
    data_[size_] = '\0';
  }

  String &operator=(const String &other) {
    if (this == &other)
      return *this;

    if (other.capacity_ == 0) {
      if (data_)
        kfree(data_);
      data_ = nullptr;
      size_ = 0;
      capacity_ = 0;
      return *this;
    }

    char *new_data = (char *)kmalloc(other.capacity_);
    if (!new_data)
      return *this;

    for (size_t i = 0; i < other.size_; i++)
      new_data[i] = other.data_[i];
    new_data[other.size_] = '\0';

    if (data_)
      kfree(data_);
    data_ = new_data;
    size_ = other.size_;
    capacity_ = other.capacity_;
    return *this;
  }

  String(String &&other)
      : size_(other.size_), capacity_(other.capacity_), data_(other.data_) {
    other.size_ = 0;
    other.capacity_ = 0;
    other.data_ = nullptr;
  }

  String &operator=(String &&other) {
    if (this == &other)
      return *this;

    if (data_)
      kfree(data_);

    size_ = other.size_;
    capacity_ = other.capacity_;
    data_ = other.data_;

    other.size_ = 0;
    other.capacity_ = 0;
    other.data_ = nullptr;

    return *this;
  }

  String &operator+=(const char *other) {
    if (!other)
      return *this;

    size_t other_len = (size_t)StringUtils::strlen(other);
    size_t new_size = size_ + other_len;

    ensure_capacity(new_size + 1);

    if (!data_)
      return *this;

    for (size_t i = 0; i < other_len; i++)
      data_[size_ + i] = other[i];

    size_ = new_size;
    data_[size_] = '\0';

    return *this;
  }

  String &operator+=(char c) {
    ensure_capacity(size_ + 2);

    if (!data_)
      return *this;

    data_[size_] = c;
    size_++;
    data_[size_] = '\0';

    return *this;
  }

  String &operator+=(const String &other) { return (*this += other.c_str()); }

  ~String() {
    if (data_)
      kfree(data_);
    data_ = nullptr;
    size_ = 0;
    capacity_ = 0;
  }

  int length() const { return (int)size_; }

  const char *c_str() const { return data_ ? data_ : ""; }

  void pop_back() {
    if (size_ == 0 || !data_)
      return;
    size_--;
    data_[size_] = '\0';
  }

  bool operator==(String &other) const {
    return StringUtils::strcmp(c_str(), other.c_str()) == 0;
  }

  bool operator==(const String &other) const {
    return StringUtils::strcmp(c_str(), other.c_str()) == 0;
  }

  bool operator!=(String &other) const { return !(*this == other); }

  bool operator!=(const String &other) const { return !(*this == other); }

  char &operator[](size_t index) {
    if (!data_ || index >= size_)
      return err_char_;
    return data_[index];
  }

  char &operator[](size_t index) const {
    if (!data_ || index >= size_)
      return err_char_;
    return data_[index];
  }

  void clear() {
    size_ = 0;
    if (data_)
      data_[0] = '\0';
  }

  String operator+(const char *other) const {
    if (!other)
      other = "";

    size_t other_len = (size_t)StringUtils::strlen(other);
    size_t new_len = size_ + other_len;

    size_t cap = new_len + 1 + DEFAULT_STRING_INCREMENT;
    char *new_data = (char *)kmalloc(cap);
    if (!new_data)
      return String(*this);

    for (size_t i = 0; i < size_; i++)
      new_data[i] = data_ ? data_[i] : '\0';
    for (size_t j = 0; j < other_len; j++)
      new_data[size_ + j] = other[j];
    new_data[new_len] = '\0';

    String result;
    if (result.data_)
      kfree(result.data_);
    result.data_ = new_data;
    result.size_ = new_len;
    result.capacity_ = cap;

    return result;
  }

  String operator+(const String &other) const {
    return (*this) + other.c_str();
  }

  String operator+(char c) const {
    size_t new_len = size_ + 1;

    size_t cap = new_len + 1 + DEFAULT_STRING_INCREMENT;
    char *new_data = (char *)kmalloc(cap);
    if (!new_data)
      return String(*this);

    for (size_t i = 0; i < size_; i++)
      new_data[i] = data_ ? data_[i] : '\0';
    new_data[size_] = c;
    new_data[new_len] = '\0';

    String result;
    if (result.data_)
      kfree(result.data_);
    result.data_ = new_data;
    result.size_ = new_len;
    result.capacity_ = cap;

    return result;
  }

  bool operator==(const char *other) const {
    return StringUtils::strcmp(c_str(), other ? other : "") == 0;
  }

  bool empty() { return size_ == 0; }
  bool empty() const { return size_ == 0; }

  void append(const String &str) { *this += str; }
  void append(const char *cstr) { *this += cstr; }

  char *begin() { return data_; }
  char *end() { return data_ + size_; }

  const char *begin() const { return data_; }
  const char *end() const { return data_ + size_; }

  const char *cbegin() const { return data_; }
  const char *cend() const { return data_ + size_; }

  void remove(size_t index) {
    if (index >= size_)
      return;
    for (size_t i = index; i < size_; i++)
      data_[i] = data_[i + 1];
    size_--;
    data_[size_] = '\0';
  }

  void insert(size_t index, char c) {
    if (index > size_)
      return;
    if (!data_)
      return;

    ensure_capacity(size_ + 2);

    for (size_t i = index; i < size_; i++)
      data_[i + 1] = data_[i];
    data_[index] = c;
    size_++;
    data_[size_] = '\0';
  }

  int to_int() const {
    if (!data_ || size_ == 0)
      return 0;

    int result = 0;
    int sign = 1;
    size_t i = 0;

    if (data_[0] == '-') {
      sign = -1;
      i++;
    }

    for (; i < size_; i++) {
      char c = data_[i];
      if (c < '0' || c > '9')
        break;
      result = result * 10 + (c - '0');
    }

    return result * sign;
  }
};

namespace StringUtils {
  inline String format(const char *fmt, ...) {
    if (!fmt)
      return String("");

    char stack_buf[256];
    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(stack_buf, sizeof(stack_buf), fmt, args);
    va_end(args);

    if (len > 0 && (size_t)len < sizeof(stack_buf))
      return String(stack_buf);

    String result;
    result.ensure_capacity(len + 1);
    if (!result.data_)
      return String("");

    va_start(args, fmt);
    vsnprintf(result.data_, len + 1, fmt, args);
    va_end(args);

    result.size_ = len;
    return result;
  }
}
