#pragma once
#include "../memory/memory.hpp"
#include "../shared/types.hpp"

constexpr int DEFAULT_VECTOR_REALLOCATION_INCREMENT = 24;

template <typename T>

class Vector {
private:
  T *data_;
  size_t size_;
  size_t capacity_;

public:
  Vector() {
    size_ = 0;
    capacity_ = DEFAULT_VECTOR_REALLOCATION_INCREMENT;
    data_ = (T *)kmalloc(sizeof(T) * capacity_);

    if (!data_)
      return;
  };

  ~Vector() {
    if (data_)
      kfree(data_);
  }

  // This constructor is broken
  Vector(const Vector &other) {
    size_ = other.size_;
    capacity_ = other.capacity_;

    data_ = (T *)kmalloc(sizeof(T) * capacity_);

    for (auto i{0uz}; i < size_; i++) {
      data_[i] = other.data_[i];
    }
  }

  Vector<T> &operator=(Vector<T> other) {
    T *temp_data_ = data_;
    size_t temp_size = size_;
    size_t temp_cap = capacity_;

    data_ = other.data_;
    size_ = other.size_;
    capacity_ = other.capacity_;

    other.data_ = temp_data_;
    other.size_ = temp_size;
    other.capacity_ = temp_cap;

    return *this;
  }

  void reallocate() {
    capacity_ += DEFAULT_VECTOR_REALLOCATION_INCREMENT;
    T *new_data_ = (T *)kmalloc(sizeof(T) * capacity_);
    if (!new_data_)
      return;
    for (int i = 0; i < size_; i++) {
      new_data_[i] = data_[i];
    }
    kfree(data_);
    data_ = new_data_;
  }

  void push_back(T val) {
    if (size_ >= capacity_)
      reallocate();
    data_[size_++] = val;
  }

  void emplace_back(T &val) {
    if (size_ >= capacity_)
      reallocate();
    data_[size_++] = val;
  }

  void pop_back() {
    if (size_ > 0)
      size_--;
  }

  void pop_front() {
    if (size_ == 0)
      return;
    for (int i = 0; i < size_ - 1; i++) {
      data_[i] = data_[i + 1];
    }
    size_--;
  }

  T &back() const { return data_[size_ - 1]; }

  T &front() const { return data_[0]; }

  bool empty() const { return size_ == 0; }

  // returning anything is invalid because we are returning a reference to T
  T &operator[](size_t index) { return data_[index]; }
  T &operator[](size_t index) const { return data_[index]; }

  void clear() { size_ = 0; }

  void reserve(size_t new_capacity_) {
    if (capacity_ >= new_capacity_)
      return;

    capacity_ = new_capacity_;
    T *new_data_ = (T *)kmalloc(capacity_ * sizeof(T));
    if (!new_data_)
      return;

    for (auto i{0uz}; i < size_; i++) {
      new_data_[i] = data_[i];
    }

    if (data_) {
      kfree(data_);
    }
    data_ = new_data_;
  }

  void shrink_to_fit() {
    if (size_ >= capacity_)
      return;
    if (!data_)
      return;
    T *new_data_ = (T *)kmalloc(sizeof(T) * size_);
    if (!new_data_)
      return;
    for (int i = 0; i < size_; i++) {
      new_data_[i] = data_[i];
    }
    kfree(data_);
    capacity_ = size_;
    data_ = new_data_;
  }

  void insert(size_t index, T &val) {
    if (size_ == 0 && index == 0) {
      data_[size_++] = val;
      return;
    }

    if (index > size_)
      return;
    if (size_ == capacity_)
      reallocate();

    if (index == size_) {
      data_[size_++] = val;
      return;
    } else {
      for (auto i{size_}; i > index; i--)
        data_[i] = data_[i - 1];
      data_[index] = val;
      return;
    }
  }

  void erase() {}

  size_t size() { return size_; };
  size_t capacity() { return capacity_; }

  T *begin() { return data_; }
  T *end() { return data_ + size_; }

  // const versions
  T *begin() const { return data_; }
  T *end() const { return data_ + size_; }
};
