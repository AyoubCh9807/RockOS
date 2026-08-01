#pragma once
#include "../memory.hpp"
#include "../utils/string_utils.hpp"
#include <stdarg.h>

class String {
private:
  size_t size;
  size_t capacity;
  char *data;
  inline static char err_char = '\0';

public:
  String(const char *cstr) {
    int length = StringUtils::strlen(cstr);
    data = (char *)kmalloc(length + 1);
    size = length;
    capacity = size + DEFAULT_BUFFER_INCREMENT;
    for (int i = 0; i < length; i++) {
      data[i] = cstr[i];
    }
    data[length] = '\0';
  };

  String() {
    size = 0;
    capacity = size + DEFAULT_BUFFER_INCREMENT;
    data = (char *)kmalloc(size + 1);
    if (data) {
      data[0] = '\0';
    }
  }

  // Rule of three: without these, the compiler-generated versions do a
  // shallow copy of `data`. That leads to two Strings owning the same heap
  // block; when one is destroyed it frees memory the other still points to
  // (use-after-free / double-free), which is what was corrupting the shell
  // input buffer on every keystroke.

  String(const String &other) {
    size = other.size;
    capacity = other.capacity;
    data = (char *)kmalloc(capacity + 1);
    if (data) {
      for (size_t i = 0; i < size; i++) {
        data[i] = other.data[i];
      }
      data[size] = '\0';
    }
  }

  String &operator=(const String &other) {
    if (this == &other) {
      return *this;
    }

    char *new_data = (char *)kmalloc(other.capacity + 1);
    if (!new_data) {
      return *this; // allocation failed, leave *this unchanged
    }

    for (size_t i = 0; i < other.size; i++) {
      new_data[i] = other.data[i];
    }
    new_data[other.size] = '\0';

    if (data != nullptr) {
      kfree(data);
    }

    data = new_data;
    size = other.size;
    capacity = other.capacity;

    return *this;
  }

  ~String() {
    if (data != nullptr) {
      kfree(data);
    }
  }

  int length() { return size; }

  // Shrinks the logical length by one, used for backspace. Unlike writing
  // '\0' through operator[], this actually updates `size` so length() and
  // future operator+ calls stay consistent.
  void pop_back() {
    if (size > 0) {
      size--;
      data[size] = '\0';
    }
  }

  String operator+(const char *other_string) {
    int len1 = size;
    int len2 = StringUtils::strlen(other_string);
    String res;
    res.size = len1 + len2;
    res.capacity = res.size + DEFAULT_BUFFER_INCREMENT;
    if (res.data)
      kfree(res.data);
    res.data = (char *)kmalloc(res.capacity + 1);
    for (int i = 0; i < len1; i++) {
      res.data[i] = this->data[i];
    }
    for (int i = 0; i < len2; i++) {
      res.data[i + len1] = other_string[i];
    }
    res.data[res.size] = '\0';
    return res;
  }

  String operator+(const String &other_string) {
    return operator+(other_string.data);
  }

  bool operator==(const char *other_string) {
    return StringUtils::strcmp(data, other_string) == 0;
  }

  const char *c_str() { return data ? data : ""; }

  char &operator[](size_t index) {
    if (index < size) {
      return data[index];
    }
    return err_char;
  }

  String operator+(char c) const {
    size_t new_len = size + 1;
    char *new_data = (char *)kmalloc(new_len + 1);
    for (size_t i = 0; i < size; i++) {
      new_data[i] = data[i];
    }
    new_data[size] = c;
    new_data[new_len] = '\0';

    String result(new_data);
    kfree(new_data);
    return result;
  }
};
