#pragma once
#include "../memory/memory.hpp"
#include "../shared/types.hpp"

constexpr int DEFAULT_VECTOR_REALLOCATION_INCREMENT = 24;

template <typename T>

class Vector {
private:
  T *data;
  size_t size;
  size_t capacity;

public:
  Vector() {
    size = 0;
    capacity = DEFAULT_VECTOR_REALLOCATION_INCREMENT;
    data = (T *)kmalloc(sizeof(T) * capacity);

    if (!data)
      return;
  };

  ~Vector() {
    if (data)
      kfree(data);
  }

  // This constructor is broken
  Vector(const Vector &other) {
    size = other.size;
    capacity = other.capacity;

    data = (T *)kmalloc(sizeof(T) * capacity);

    for (auto i{0uz}; i < size; i++) {
      data[i] = other.data[i];
    }
  }

  Vector<T> &operator=(Vector<T> other) {
    T *temp_data = data;
    size_t temp_size = size;
    size_t temp_cap = capacity;

    data = other.data;
    size = other.size;
    capacity = other.capacity;

    other.data = temp_data;
    other.size = temp_size;
    other.capacity = temp_cap;

    return *this;
  }

  void reallocate() {
    capacity += DEFAULT_VECTOR_REALLOCATION_INCREMENT;
    T *new_data = (T *)kmalloc(sizeof(T) * capacity);
    if (!new_data)
      return;
    for (int i = 0; i < size; i++) {
      new_data[i] = data[i];
    }
    kfree(data);
    data = new_data;
  }

  void push_back(T val) {
    if (size >= capacity)
      reallocate();
    data[size++] = val;
  }

  void emplace_back(T &val) {
    if (size >= capacity)
      reallocate();
    data[size++] = val;
  }

  void pop_back() {
    if (size > 0)
      size--;
  }

  void pop_front() {
    if (size == 0)
      return;
    for (int i = 0; i < size - 1; i++) {
      data[i] = data[i + 1];
    }
    size--;
  }

  T &back() { return data[size - 1]; }

  T &front() { return data[0]; }

  bool empty() { return size == 0; }

  // returning anything is invalid because we are returning a reference to T
  T &operator[](size_t index) { return data[index]; }
  T &operator[](size_t index) const { return data[index]; }

  void clear() { size = 0; }

  void reserve(size_t new_capacity) {
    if (capacity >= new_capacity)
      return;

    capacity = new_capacity;
    T *new_data = (T *)kmalloc(capacity * sizeof(T));
    if (!new_data)
      return;

    for (auto i{0uz}; i < size; i++) {
      new_data[i] = data[i];
    }

    if (data) {
      kfree(data);
    }
    data = new_data;
  }

  void shrink_to_fit() {
    if (size >= capacity)
      return;
    if (!data)
      return;
    T *new_data = (T *)kmalloc(sizeof(T) * size);
    if (!new_data)
      return;
    for (int i = 0; i < size; i++) {
      new_data[i] = data[i];
    }
    kfree(data);
    capacity = size;
    data = new_data;
  }

  void insert(size_t index, T &val) {
    if (size == 0 && index == 0) {
      data[size++] = val;
      return;
    }

    if (index > size)
      return;
    if (size == capacity)
      reallocate();

    if (index == size) {
      data[size++] = val;
      return;
    } else {
      for (auto i{size}; i > index; i--)
        data[i] = data[i - 1];
      data[index] = val;
      return;
    }
  }

  void erase() {}

  size_t get_size() { return size; };
  size_t get_capacity() { return capacity; }
};
