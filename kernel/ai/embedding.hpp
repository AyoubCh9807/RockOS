#pragma once

#include "../containers/vector.hpp"

class Embedding {
private:
  static constexpr int EMBEDDING_DIMENSION = 128;

  int vocabulary_size;
  Vector<float> matrix;
  Vector<float> input;

  // Accumulated gradient for every weight in matrix. Same shape as
  // matrix, reset to zero at the start of every training step.
  Vector<float> d_matrix;

public:
  Embedding(int vocabulary_size) : vocabulary_size(vocabulary_size) {
    int total_values = vocabulary_size * EMBEDDING_DIMENSION;

    matrix.resize(total_values, 0.0f);
    d_matrix.resize(total_values, 0.0f);
  }
  float *get_embedding(int token_id) {
    if (token_id < 0 || token_id >= vocabulary_size)
      return nullptr;

    int offset = token_id * EMBEDDING_DIMENSION;
    return &matrix[offset];
  }

  int dimension() const { return EMBEDDING_DIMENSION; }

  int size() const { return vocabulary_size; }

  // Backpropagation 

  // We call this once per token that was looked up during the forward pass,
  // passing in the gradient of the loss with respect to that token's
  // embedding vector (128 numbers). Only the row for that specific token
  // gets updated, every other word in the vocabulary is untouched.
  void backward(int token_id, const Vector<float> &d_embedding_output) {
    int offset = token_id * EMBEDDING_DIMENSION;

    for (int j = 0; j < EMBEDDING_DIMENSION; j++)
      d_matrix[offset + j] += d_embedding_output[j];
  }

  // Clears out gradients accumulated from a previous training step.
  // Always call this before running forward+backward on a new example
  // (or new batch), otherwise gradients from old examples keep piling up.
  void zero_grad() {
    for (int i = 0; i < d_matrix.size(); i++)
      d_matrix[i] = 0.0f;
  }

  // Applies one step of plain gradient descent using whatever gradients
  // have been accumulated so far via backward().
  void apply_gradients(float learning_rate) {
    for (int i = 0; i < matrix.size(); i++)
      matrix[i] -= learning_rate * d_matrix[i];
  }

  // Kept for backwards compatibility with any code that already calls this.
  void update_weights(const Vector<float> &gradients, float learning_rate) {
    for (int i = 0; i < matrix.size(); i++)
      matrix[i] -= learning_rate * gradients[i];
  }

    int parameter_count() const {
    return matrix.size();
  }

  void export_weights(Vector<float> &out) const {
    for (int i = 0; i < matrix.size(); i++)
      out.push_back(matrix[i]);
  }

  bool import_weights(const Vector<float> &in, int &cursor) {
    if (cursor + matrix.size() > in.size())
      return false;

    for (int i = 0; i < matrix.size(); i++)
      matrix[i] = in[cursor++];

    return true;
  }
};
