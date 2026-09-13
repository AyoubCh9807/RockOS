#pragma once

#include "../containers/vector.hpp"
#include "../utils/math_utils.hpp"
#include "../random/random.hpp"

class Embedding {
private:
  static constexpr int EMBEDDING_DIMENSION = 128;

  int vocabulary_size;

  Vector<float> matrix;
  Vector<float> d_matrix;

  // Max L2 norm allowed for the accumulated embedding gradient before
  // it's applied. See SelfAttention::GRADIENT_CLIP for the full
  // reasoning. Note d_matrix accumulates gradients for every token seen
  // in a batch, so its norm is naturally larger than a single-vector
  // gradient - it's clipped as one whole vector here for simplicity,
  // matching how it's applied below.
  static constexpr float GRADIENT_CLIP = 1.0f;

  static float random_weight(float limit) {
    u32 value = Random::next();

    float normalized = (float)value / 4294967295.0f;

    return -limit + normalized * (2.0f * limit);
  }

  // Clips a gradient vector in place so its L2 norm never exceeds
  // max_norm. Same policy as Trainer::clip_gradient, duplicated here so
  // the embedding table's gradient gets bounded too, not just the
  // classifier's output gradient.
  static void clip_gradient(Vector<float> &grad, float max_norm) {
    float norm_sq = 0.0f;

    for (int i = 0; i < grad.size(); i++)
      norm_sq += grad[i] * grad[i];

    float norm = MathUtils::sqrt(norm_sq);

    if (norm > max_norm && norm > 0.0f) {
      float scale = max_norm / norm;

      for (int i = 0; i < grad.size(); i++)
        grad[i] *= scale;
    }
  }

public:
  Embedding(int vocabulary_size) : vocabulary_size(vocabulary_size) {

    int total_values = vocabulary_size * EMBEDDING_DIMENSION;

    matrix.resize(total_values, 0.0f);
    d_matrix.resize(total_values, 0.0f);

    float limit =
        MathUtils::sqrt(
            6.0f /
            (float)(vocabulary_size + EMBEDDING_DIMENSION));

    for (int i = 0; i < matrix.size(); i++)
      matrix[i] = random_weight(limit);
  }

  float *get_embedding(int token_id) {

    if (token_id < 0 || token_id >= vocabulary_size)
      return nullptr;

    int offset = token_id * EMBEDDING_DIMENSION;

    return &matrix[offset];
  }

  int dimension() const {
    return EMBEDDING_DIMENSION;
  }

  int size() const {
    return vocabulary_size;
  }

  void backward(
      int token_id,
      const Vector<float> &d_embedding_output) {

    int offset = token_id * EMBEDDING_DIMENSION;

    for (int j = 0; j < EMBEDDING_DIMENSION; j++)
      d_matrix[offset + j] += d_embedding_output[j];
  }

  void zero_grad() {

    for (int i = 0; i < d_matrix.size(); i++)
      d_matrix[i] = 0.0f;
  }

  // See SelfAttention::collect_gradients for why this exists: lets
  // Trainer fold this module's gradient into one combined global-norm
  // clip across the whole network, instead of relying only on the
  // per-vector clip in apply_gradients() above.
  void collect_gradients(Vector<Vector<float> *> &out) {
    out.push_back(&d_matrix);
  }

  void apply_gradients(float learning_rate) {
    // Bound the gradient's L2 norm before it ever touches the
    // embedding table. Without this, a token that shows up often in one
    // batch (or one with skewed gradients) can push its embedding row -
    // and everything downstream that reads it - out to extreme values.
    clip_gradient(d_matrix, GRADIENT_CLIP);

    for (int i = 0; i < matrix.size(); i++)
      matrix[i] -= learning_rate * d_matrix[i];
  }

  void update_weights(
      const Vector<float> &gradients,
      float learning_rate) {

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

  bool import_weights(
      const Vector<float> &in,
      int &cursor) {

    if (cursor + matrix.size() > in.size())
      return false;

    for (int i = 0; i < matrix.size(); i++)
      matrix[i] = in[cursor++];

    return true;
  }
};
