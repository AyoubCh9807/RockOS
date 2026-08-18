#pragma once

#include "../memory/heap.hpp"
#include "../shared/types.hpp"
#include "../utils/string_utils.hpp"

constexpr int DEFAULT_STRING_INCREMENT = 16;

class String {
private:
  size_t size_;
  size_t capacity_;
  char *data_;

  static inline char err_char_ = '\0';

  void ensure_capacity(size_t needed) {
    // needed includes space for '\0'
    if (needed <= capacity_)
      return;

    size_t new_cap = capacity_;
    if (new_cap == 0)
      new_cap = DEFAULT_STRING_INCREMENT;
    while (new_cap < needed)
      new_cap += DEFAULT_STRING_INCREMENT;

    char *new_data = (char *)kmalloc(new_cap);
    if (!new_data)
      return; // keep old buffer if allocation fails

    // copy old
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

  // Copy ctor (deep copy)
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

  // Copy assignment (deep copy, handles self-assign)
  String &operator=(const String &other) {
    if (this == &other)
      return *this;

    if (other.capacity_ == 0) {
      // free current
      if (data_)
        kfree(data_);
      data_ = nullptr;
      size_ = 0;
      capacity_ = 0;
      return *this;
    }

    char *new_data = (char *)kmalloc(other.capacity_);
    if (!new_data)
      return *this; // leave unchanged on alloc failure

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

  // operator==
  bool operator==(String &other) const {
    return StringUtils::strcmp(c_str(), other.c_str()) == 0;
  }

  // operator== for const string
  bool operator==(const String &other) const {
    return StringUtils::strcmp(c_str(), other.c_str()) == 0;
  }

  // operator!=
  bool operator!=(String &other) const { return !(*this == other); }

  // operator!= for const string
  bool operator!=(const String &other) const { return !(*this == other); }

  // operator[] (safe-ish)
  char &operator[](size_t index) {
    if (!data_ || index >= size_)
      return err_char_;
    return data_[index];
  }

  // operator[] const (safe-ish)
  char &operator[](size_t index) const {
    if (!data_ || index >= size_)
      return err_char_;
    return data_[index];
  }

  // Clear the string contents
  void clear() {
    size_ = 0;

    if (data_)
      data_[0] = '\0';
  }

  // Append a C-string (returns new String)
  String operator+(const char *other) const {
    if (!other)
      other = "";

    size_t other_len = (size_t)StringUtils::strlen(other);
    size_t new_len = size_ + other_len;

    // allocate exact-ish
    size_t cap = new_len + 1 + DEFAULT_STRING_INCREMENT;
    char *new_data = (char *)kmalloc(cap);
    if (!new_data) {
      // allocation failed: return a copy of *this (safe fallback)
      return String(*this);
    }

    for (size_t i = 0; i < size_; i++)
      new_data[i] = data_ ? data_[i] : '\0';
    for (size_t j = 0; j < other_len; j++)
      new_data[size_ + j] = other[j];
    new_data[new_len] = '\0';

    String result;
    // replace result's buffer with ours
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

  // The one your warning references
  String operator+(char c) const {
    size_t new_len = size_ + 1;

    size_t cap = new_len + 1 + DEFAULT_STRING_INCREMENT;
    char *new_data = (char *)kmalloc(cap);
    if (!new_data) {
      return String(*this); // avoid writing through nullptr
    }

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

  // Non-const iterators
  char *begin() { return data_; }
  char *end() { return data_ + size_; }

  // Const iterators for range-based for loops on const Strings
  const char *begin() const { return data_; }
  const char *end() const { return data_ + size_; }

  const char *cbegin() const { return data_; }
  const char *cend() const { return data_ + size_; }

  void remove(size_t index) {
    if (index >= size_)
      return;
    // Hello world becomes:
    // Hell world
    // everything on the right of the deleted o gets shifted one spot to the
    // left
    for (size_t i = index; i < size_; i++) {
      data_[i] = data_[i + 1];
    }

    size_--;

    data_[size_] = '\0';
  }

  void insert(size_t index, char c) {
    if (index > size_)
      return;

    if (!data_)
      return;

    ensure_capacity(size_ + 2);

    // hello world becomes:
    // hellow world
    // everything on the right of the new character gets shifted by one spot to
    // the right

    for (size_t i = index; i < size_; i++) {
      data_[i + 1] = data_[i];
    }
    data_[index] = c;
    size_++;
    data_[size_] = '\0';
  }

  static String format(const char *fmt, ...) {
    if (!fmt)
      return String("");

    va_list args;
    va_start(args, fmt);

    String result;

    for (int i = 0; fmt[i] != '\0'; i++) {
      if (fmt[i] != '%' || fmt[i + 1] == '\0') {
        result += fmt[i];
        continue;
      }

      i++;

      switch (fmt[i]) {
      case '%':
        result += '%';
        break;

      case 's': {
        const char *s = va_arg(args, const char *);
        if (s)
          result += s;
        break;
      }

      case 'd': {
        int n = va_arg(args, int);
        const char *num = StringUtils::iota(n);

        result += num;

        // iota() allocates!
        if (num && num[0] != '\0')
          kfree((void *)num);

        break;
      }

      case 'c': {
        char c = (char)va_arg(args, int);
        result += c;
        break;
      }

      default:
        result += fmt[i];
        break;
      }
    }

    va_end(args);
    return result;
  }
};
