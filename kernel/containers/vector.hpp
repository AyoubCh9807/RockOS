#pragma once

#include "../memory/heap.hpp"
#include "../shared/types.hpp"

constexpr int DEFAULT_VECTOR_REALLOCATION_INCREMENT = 24;

template <typename T>
class Vector {
private:
  T *data_;
  size_t size_;
  size_t capacity_;

  void destroy_elements() {
    for (size_t i = 0; i < size_; i++)
      data_[i].~T();

    size_ = 0;
  }

  bool move_to_new_storage(size_t new_capacity) {
    T *new_data = (T *)kmalloc(sizeof(T) * new_capacity);

    if (!new_data)
      return false;

    for (size_t i = 0; i < size_; i++) {
      new (&new_data[i]) T(data_[i]);
      data_[i].~T();
    }

    kfree(data_);

    data_ = new_data;
    capacity_ = new_capacity;

    return true;
  }

public:
  Vector()
      : data_(nullptr),
        size_(0),
        capacity_(DEFAULT_VECTOR_REALLOCATION_INCREMENT) {

    data_ = (T *)kmalloc(sizeof(T) * capacity_);

    if (!data_)
      capacity_ = 0;
  }

  ~Vector() {
    if (!data_)
      return;

    destroy_elements();
    kfree(data_);
  }

  Vector(const Vector &other)
      : data_(nullptr),
        size_(0),
        capacity_(0) {

    if (other.capacity_ == 0)
      return;

    data_ = (T *)kmalloc(sizeof(T) * other.capacity_);

    if (!data_)
      return;

    capacity_ = other.capacity_;

    for (size_t i = 0; i < other.size_; i++) {
      new (&data_[i]) T(other.data_[i]);
      size_++;
    }
  }

  Vector<T> &operator=(Vector<T> other) {
    T *old_data = data_;
    size_t old_size = size_;
    size_t old_capacity = capacity_;

    data_ = other.data_;
    size_ = other.size_;
    capacity_ = other.capacity_;

    other.data_ = old_data;
    other.size_ = old_size;
    other.capacity_ = old_capacity;

    return *this;
  }

  void reallocate() {
    size_t new_capacity =
        capacity_ + DEFAULT_VECTOR_REALLOCATION_INCREMENT;

    if (new_capacity <= capacity_)
      return;

    move_to_new_storage(new_capacity);
  }

  void push_back(const T &val) {
    if (size_ >= capacity_) {
      reallocate();

      if (size_ >= capacity_)
        return;
    }

    new (&data_[size_]) T(val);
    size_++;
  }

  void emplace_back(T &val) {
    if (size_ >= capacity_) {
      reallocate();

      if (size_ >= capacity_)
        return;
    }

    new (&data_[size_]) T(val);
    size_++;
  }

  void pop_back() {
    if (size_ == 0)
      return;

    data_[size_ - 1].~T();
    size_--;
  }

  void pop_front() {
    if (size_ == 0)
      return;

    data_[0].~T();

    for (size_t i = 0; i + 1 < size_; i++) {
      new (&data_[i]) T(data_[i + 1]);
      data_[i + 1].~T();
    }

    size_--;
  }

  T &back() const {
    return data_[size_ - 1];
  }

  T &front() const {
    return data_[0];
  }

  bool empty() const {
    return size_ == 0;
  }

  T &operator[](size_t index) {
    return data_[index];
  }

  T &operator[](size_t index) const {
    return data_[index];
  }

  void clear() {
    destroy_elements();
  }

  void reserve(size_t new_capacity) {
    if (new_capacity <= capacity_)
      return;

    move_to_new_storage(new_capacity);
  }

  void shrink_to_fit() {
    if (size_ >= capacity_)
      return;

    if (size_ == 0) {
      if (data_)
        kfree(data_);

      data_ = nullptr;
      capacity_ = 0;
      return;
    }

    move_to_new_storage(size_);
  }

  void insert(size_t index, const T &val) {
    if (index > size_)
      return;

    if (size_ >= capacity_) {
      reallocate();

      if (size_ >= capacity_)
        return;
    }

    if (index == size_) {
      new (&data_[size_]) T(val);
      size_++;
      return;
    }

    new (&data_[size_]) T(data_[size_ - 1]);

    for (size_t i = size_ - 1; i > index; i--) {
      data_[i].~T();
      new (&data_[i]) T(data_[i - 1]);
    }

    data_[index].~T();
    new (&data_[index]) T(val);

    size_++;
  }

  void erase(int index) {
    if (index < 0 || (size_t)index >= size_)
      return;

    size_t current = (size_t)index;

    data_[current].~T();

    for (size_t i = current; i + 1 < size_; i++) {
      new (&data_[i]) T(data_[i + 1]);
      data_[i + 1].~T();
    }

    size_--;
  }

  size_t size() {
    return size_;
  }

  size_t size() const {
    return size_;
  }

  size_t capacity() {
    return capacity_;
  }

  size_t capacity() const {
    return capacity_;
  }

  T *begin() {
    return data_;
  }

  T *end() {
    return data_ + size_;
  }

  const T *begin() const {
    return data_;
  }

  const T *end() const {
    return data_ + size_;
  }

  void fill(const T &value) {
    for (size_t i = 0; i < size_; i++)
      data_[i] = value;
  }

  T sum() const {
    T s{};

    for (size_t i = 0; i < size_; i++)
      s += data_[i];

    return s;
  }

  T max() const {
    if (!data_ || size_ == 0)
      return T{};

    T m = data_[0];

    for (size_t i = 1; i < size_; i++) {
      if (data_[i] > m)
        m = data_[i];
    }

    return m;
  }

  T min() const {
    if (!data_ || size_ == 0)
      return T{};

    T m = data_[0];

    for (size_t i = 1; i < size_; i++) {
      if (data_[i] < m)
        m = data_[i];
    }

    return m;
  }

  template <typename Ty>
  void resize(size_t new_size, Ty value = Ty{}) {
    if (new_size > capacity_) {
      reserve(new_size);

      if (new_size > capacity_)
        return;
    }

    while (size_ < new_size)
      push_back(value);

    while (size_ > new_size)
      pop_back();
  }
};
