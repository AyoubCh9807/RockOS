#pragma once

#include "../containers/vector.hpp"
#include "../random/random.hpp"
#include "../utils/math_utils.hpp"

#include <cmath>

class FeedForwardNetwork {
private:
  static constexpr int INPUT_DIMENSION = 128;
  static constexpr int HIDDEN_DIMENSION = 256;
  static constexpr int OUTPUT_DIMENSION = 128;

  // Same reasoning as IntentClassifier / SelfAttention: without decay,
  // these weights can grow unbounded over many epochs and eventually
  // feed extreme values into the rest of the network.
  static constexpr float WEIGHT_DECAY = 0.01f;

  // Max L2 norm allowed for any single gradient vector before it's
  // applied. See SelfAttention::GRADIENT_CLIP for the full reasoning -
  // weight decay alone doesn't stop a single large gradient step from
  // pushing weights (and therefore hidden activations) into a regime
  // that saturates everything downstream.
  static constexpr float GRADIENT_CLIP = 1.0f;

  Vector<float> first_weights;
  Vector<float> first_bias;

  Vector<float> second_weights;
  Vector<float> second_bias;

  Vector<float> d_first_weights;
  Vector<float> d_first_bias;
  Vector<float> d_second_weights;
  Vector<float> d_second_bias;

  float relu(float value) { return value > 0.0f ? value : 0.0f; }

  // Clips a gradient vector in place so its L2 norm never exceeds
  // max_norm. Same policy as Trainer::clip_gradient, duplicated here so
  // every parameter gradient in this module gets bounded, not just the
  // classifier's output gradient.
  //
  // NaN-safety: comparisons against NaN are always false in C++, so the
  // old "if (norm > max_norm)" silently did nothing when norm was NaN -
  // a corrupted gradient sailed straight through "clipping" untouched.
  // We now explicitly detect non-finite norms first and zero the whole
  // vector in that case, since there's no sane scale factor to apply.
  static void clip_gradient(Vector<float> &grad, float max_norm) {
    float norm_sq = 0.0f;

    for (int i = 0; i < grad.size(); i++)
      norm_sq += grad[i] * grad[i];

    float norm = MathUtils::sqrt(norm_sq);

    if (!std::isfinite(norm)) {
      for (int i = 0; i < grad.size(); i++)
        grad[i] = 0.0f;

      return;
    }

    if (norm > max_norm && norm > 0.0f) {
      float scale = max_norm / norm;

      for (int i = 0; i < grad.size(); i++)
        grad[i] *= scale;
    }
  }

public:
  FeedForwardNetwork() {
    first_weights.reserve(INPUT_DIMENSION * HIDDEN_DIMENSION);
    first_bias.reserve(HIDDEN_DIMENSION);

    second_weights.reserve(HIDDEN_DIMENSION * OUTPUT_DIMENSION);
    second_bias.reserve(OUTPUT_DIMENSION);

    for (int i = 0; i < INPUT_DIMENSION * HIDDEN_DIMENSION; i++)
      first_weights.push_back(
          (static_cast<float>(Random::next() % 2001) - 1000.0f) / 10000.0f);

    for (int i = 0; i < HIDDEN_DIMENSION; i++)
      first_bias.push_back(0.0f);

    for (int i = 0; i < HIDDEN_DIMENSION * OUTPUT_DIMENSION; i++)
      second_weights.push_back(
          (static_cast<float>(Random::next() % 2001) - 1000.0f) / 10000.0f);

    for (int i = 0; i < OUTPUT_DIMENSION; i++)
      second_bias.push_back(0.0f);

    d_first_weights.resize(INPUT_DIMENSION * HIDDEN_DIMENSION, 0.0f);

    d_first_bias.resize(HIDDEN_DIMENSION, 0.0f);

    d_second_weights.resize(HIDDEN_DIMENSION * OUTPUT_DIMENSION, 0.0f);

    d_second_bias.resize(OUTPUT_DIMENSION, 0.0f);
  }

  Vector<float> forward(const Vector<float> &input) {
    Vector<float> hidden;
    hidden.reserve(HIDDEN_DIMENSION);

    for (int i = 0; i < HIDDEN_DIMENSION; i++) {
      float sum = 0.0f;

      for (int j = 0; j < INPUT_DIMENSION; j++) {
        int weight_index = j * HIDDEN_DIMENSION + i;

        sum += input[j] * first_weights[weight_index];
      }

      sum += first_bias[i];

      hidden.push_back(relu(sum));
    }

    Vector<float> output;
    output.reserve(OUTPUT_DIMENSION);

    for (int i = 0; i < OUTPUT_DIMENSION; i++) {
      float sum = 0.0f;

      for (int j = 0; j < HIDDEN_DIMENSION; j++) {
        int weight_index = j * OUTPUT_DIMENSION + i;

        sum += hidden[j] * second_weights[weight_index];
      }

      sum += second_bias[i];

      output.push_back(sum);
    }

    return output;
  }

  Vector<float> backward(const Vector<float> &dout,
                         const Vector<float> &input) {
    Vector<float> hidden_pre;
    Vector<float> hidden;

    hidden_pre.resize(HIDDEN_DIMENSION, 0.0f);
    hidden.resize(HIDDEN_DIMENSION, 0.0f);

    for (int i = 0; i < HIDDEN_DIMENSION; i++) {
      float sum = 0.0f;
      for (int j = 0; j < INPUT_DIMENSION; j++) {
        int weight_index = j * HIDDEN_DIMENSION + i;
        sum += input[j] * first_weights[weight_index];
      }
      sum += first_bias[i];

      hidden_pre[i] = sum;
      hidden[i] = relu(sum);
    }

    Vector<float> d_hidden;
    d_hidden.resize(HIDDEN_DIMENSION, 0.0f);

    for (int i = 0; i < OUTPUT_DIMENSION; i++) {
      d_second_bias[i] += dout[i];

      for (int j = 0; j < HIDDEN_DIMENSION; j++) {
        int weight_index = j * OUTPUT_DIMENSION + i;

        d_second_weights[weight_index] += hidden[j] * dout[i];
        d_hidden[j] += dout[i] * second_weights[weight_index];
      }
    }

    Vector<float> d_hidden_pre;
    d_hidden_pre.resize(HIDDEN_DIMENSION, 0.0f);

    for (int j = 0; j < HIDDEN_DIMENSION; j++)
      d_hidden_pre[j] = hidden_pre[j] > 0.0f ? d_hidden[j] : 0.0f;

    Vector<float> d_input;
    d_input.resize(INPUT_DIMENSION, 0.0f);

    for (int j = 0; j < HIDDEN_DIMENSION; j++) {
      d_first_bias[j] += d_hidden_pre[j];

      for (int i = 0; i < INPUT_DIMENSION; i++) {
        int weight_index = i * HIDDEN_DIMENSION + j;

        d_first_weights[weight_index] += input[i] * d_hidden_pre[j];
        d_input[i] += d_hidden_pre[j] * first_weights[weight_index];
      }
    }

    return d_input;
  }

  // See SelfAttention::collect_gradients for why this exists: lets
  // Trainer fold this module's gradients into one combined global-norm
  // clip across the whole network, instead of relying only on the
  // per-vector clip in apply_gradients() below.
  void collect_gradients(Vector<Vector<float> *> &out) {
    out.push_back(&d_first_weights);
    out.push_back(&d_first_bias);
    out.push_back(&d_second_weights);
    out.push_back(&d_second_bias);
  }

  void zero_grad() {
    for (int i = 0; i < d_first_weights.size(); i++)
      d_first_weights[i] = 0.f;
    for (int i = 0; i < d_first_bias.size(); i++)
      d_first_bias[i] = 0.f;
    for (int i = 0; i < d_second_weights.size(); i++)
      d_second_weights[i] = 0.f;
    for (int i = 0; i < d_second_bias.size(); i++)
      d_second_bias[i] = 0.f;
  }

  void apply_gradients(float learning_rate) {
    // Bound each gradient's L2 norm before it ever touches the weights.
    // Weight decay below only pulls weights gently toward zero every
    // step; it does not cap how much a single large gradient can push
    // them in one step, which is what let this network explode.
    clip_gradient(d_first_weights, GRADIENT_CLIP);
    clip_gradient(d_first_bias, GRADIENT_CLIP);
    clip_gradient(d_second_weights, GRADIENT_CLIP);
    clip_gradient(d_second_bias, GRADIENT_CLIP);

    // Same decoupled-in-formula L2 decay pattern as IntentClassifier /
    // SelfAttention. Biases are left undecayed on purpose.
    for (int i = 0; i < first_weights.size(); i++)
      first_weights[i] -=
          learning_rate * (d_first_weights[i] + WEIGHT_DECAY * first_weights[i]);

    for (int i = 0; i < first_bias.size(); i++)
      first_bias[i] -= learning_rate * d_first_bias[i];

    for (int i = 0; i < second_weights.size(); i++)
      second_weights[i] -=
          learning_rate * (d_second_weights[i] + WEIGHT_DECAY * second_weights[i]);

    for (int i = 0; i < second_bias.size(); i++)
      second_bias[i] -= learning_rate * d_second_bias[i];
  }

  void update_weights(const Vector<float> &first_weight_gradients,
                      const Vector<float> &first_bias_gradients,
                      const Vector<float> &second_weight_gradients,
                      const Vector<float> &second_bias_gradients,
                      float learning_rate) {

    for (int i = 0; i < first_weights.size(); i++)
      first_weights[i] -= learning_rate * first_weight_gradients[i];

    for (int i = 0; i < first_bias.size(); i++)
      first_bias[i] -= learning_rate * first_bias_gradients[i];

    for (int i = 0; i < second_weights.size(); i++)
      second_weights[i] -= learning_rate * second_weight_gradients[i];

    for (int i = 0; i < second_bias.size(); i++)
      second_bias[i] -= learning_rate * second_bias_gradients[i];
  }

  int parameter_count() const {
    return first_weights.size() + first_bias.size() + second_weights.size() +
           second_bias.size();
  }

  void export_weights(Vector<float> &out) const {
    for (int i = 0; i < first_weights.size(); i++)
      out.push_back(first_weights[i]);

    for (int i = 0; i < first_bias.size(); i++)
      out.push_back(first_bias[i]);

    for (int i = 0; i < second_weights.size(); i++)
      out.push_back(second_weights[i]);

    for (int i = 0; i < second_bias.size(); i++)
      out.push_back(second_bias[i]);
  }

  bool import_weights(const Vector<float> &in, int &cursor) {
    int count = parameter_count();

    if (cursor + count > in.size())
      return false;

    for (int i = 0; i < first_weights.size(); i++)
      first_weights[i] = in[cursor++];

    for (int i = 0; i < first_bias.size(); i++)
      first_bias[i] = in[cursor++];

    for (int i = 0; i < second_weights.size(); i++)
      second_weights[i] = in[cursor++];

    for (int i = 0; i < second_bias.size(); i++)
      second_bias[i] = in[cursor++];

    return true;
  }
};
