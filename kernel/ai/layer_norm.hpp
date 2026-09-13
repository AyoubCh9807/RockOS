#pragma once

#include "../containers/vector.hpp"
#include "../utils/math_utils.hpp"

class LayerNorm {
private:
  static constexpr int DIMENSION = 128;

  Vector<float> gamma;
  Vector<float> beta;

  float epsilon = 0.0001f;

  // Accumulated gradients w.r.t gamma and beta.
  Vector<float> d_gamma;
  Vector<float> d_beta;

  // Max L2 norm allowed for any single gradient vector before it's
  // applied. See SelfAttention::GRADIENT_CLIP for the full reasoning.
  static constexpr float GRADIENT_CLIP = 1.0f;

  // Small L2 penalty applied to gamma only, same pattern as every other
  // weight matrix in the model (SelfAttention, FeedForwardNetwork,
  // IntentClassifier). This was previously missing here: gamma directly
  // rescales every activation flowing through this layer, and with only
  // gradient clipping (which bounds step SIZE, not long-term drift) and
  // no decay, gamma could still creep upward for as long as the
  // gradient consistently pointed the same way - which is exactly what
  // let this model diverge again around epoch 35 even after clipping
  // was added everywhere else. beta is left undecayed on purpose, same
  // reasoning as biases elsewhere: regularizing an additive shift term
  // doesn't help and can hurt.
  static constexpr float WEIGHT_DECAY = 0.01f;

  // Clips a gradient vector in place so its L2 norm never exceeds
  // max_norm. Same policy as Trainer::clip_gradient, duplicated here so
  // every parameter gradient in this module gets bounded, not just the
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
  LayerNorm() {
    gamma.resize(DIMENSION, 1.0f);
    beta.resize(DIMENSION, 0.0f);

    d_gamma.resize(DIMENSION, 0.0f);
    d_beta.resize(DIMENSION, 0.0f);
  }

  Vector<float> normalize(const Vector<float> &input) {
    if (input.size() != DIMENSION)
      return Vector<float>();

    Vector<float> output;
    output.resize(DIMENSION, 0.0f);

    float input_sum = input.sum();
    float mean = input_sum / DIMENSION;

    float squared_diff_sum = 0.0f;

    for (int i = 0; i < DIMENSION; i++) {
      squared_diff_sum += MathUtils::pow(input[i] - mean, 2.0f);
    }

    float variance = squared_diff_sum / DIMENSION;
    float invstd = 1.0f / MathUtils::sqrt(variance + epsilon);

    for (int i = 0; i < DIMENSION; i++) {
      float xhat = (input[i] - mean) * invstd;

      output[i] = xhat * gamma[i] + beta[i];
    }

    return output;
  }

  // Backpropagation 

  // dout is the gradient of the loss w.r.t. this layer's output.
  // input MUST be the exact same vector that was passed into normalize()
  // during the forward pass (we recompute mean/variance from it instead of
  // caching them, so this class stays safe to reuse across many tokens).
  //
  // Returns the gradient w.r.t. this layer's input, and accumulates
  // gradients for gamma/beta internally (read by apply_gradients()).
  Vector<float> backward(const Vector<float> &dout,
                         const Vector<float> &input) {
    int N = input.size();

    float mean = input.sum() / N;
    float squared_diff_sum = 0.f;
    for (int j = 0; j < N; j++)
      squared_diff_sum += MathUtils::pow(input[j] - mean, 2.f);
    float variance = squared_diff_sum / N;
    float invstd = 1.0f / MathUtils::sqrt(variance + epsilon);

    Vector<float> xhat;
    xhat.resize(N, 0.0f);
    for (int i = 0; i < N; i++)
      xhat[i] = (input[i] - mean) * invstd;

    Vector<float> dxhat;
    dxhat.resize(N, 0.0f);

    float sum_dxhat = 0.f;
    float sum_dxhat_xhat = 0.f;

    for (int i = 0; i < N; i++) {
      d_gamma[i] += dout[i] * xhat[i];
      d_beta[i] += dout[i];

      float dxh = dout[i] * gamma[i];
      dxhat[i] = dxh;
      sum_dxhat += dxh;
      sum_dxhat_xhat += dxh * xhat[i];
    }

    // Standard LayerNorm backward formula.
    Vector<float> dx;
    dx.resize(N, 0.0f);

    for (int i = 0; i < N; i++) {
      dx[i] =
          (invstd / N) * (N * dxhat[i] - sum_dxhat - xhat[i] * sum_dxhat_xhat);
    }

    return dx;
  }

  // See SelfAttention::collect_gradients for why this exists: lets
  // Trainer fold this module's gradients into one combined global-norm
  // clip across the whole network, instead of relying only on the
  // per-vector clip in apply_gradients() below.
  void collect_gradients(Vector<Vector<float> *> &out) {
    out.push_back(&d_gamma);
    out.push_back(&d_beta);
  }

  void zero_grad() {
    for (int i = 0; i < d_gamma.size(); i++)
      d_gamma[i] = 0.f;
    for (int i = 0; i < d_beta.size(); i++)
      d_beta[i] = 0.f;
  }

  void apply_gradients(float learning_rate) {
    // Bound each gradient's L2 norm before it ever touches gamma/beta.
    clip_gradient(d_gamma, GRADIENT_CLIP);
    clip_gradient(d_beta, GRADIENT_CLIP);

    // gamma gets decay (see WEIGHT_DECAY comment above) so it can't
    // drift upward indefinitely even under a persistent gradient
    // direction; beta does not, matching the bias convention used
    // everywhere else in this model.
    for (int i = 0; i < gamma.size(); i++)
      gamma[i] -= learning_rate * (d_gamma[i] + WEIGHT_DECAY * gamma[i]);

    for (int i = 0; i < beta.size(); i++)
      beta[i] -= learning_rate * d_beta[i];
  }

  // Kept for backwards compatibility.
  void update_weights(const Vector<float> &gamma_gradients,
                      const Vector<float> &beta_gradients,
                      float learning_rate) {

    for (int i = 0; i < gamma.size(); i++)
      gamma[i] -= learning_rate * gamma_gradients[i];

    for (int i = 0; i < beta.size(); i++)
      beta[i] -= learning_rate * beta_gradients[i];
  }

  int parameter_count() const { return gamma.size() + beta.size(); }

  void export_weights(Vector<float> &out) const {
    for (int i = 0; i < gamma.size(); i++)
      out.push_back(gamma[i]);

    for (int i = 0; i < beta.size(); i++)
      out.push_back(beta[i]);
  }

  bool import_weights(const Vector<float> &in, int &cursor) {
    int count = parameter_count();

    if (cursor + count > in.size())
      return false;

    for (int i = 0; i < gamma.size(); i++)
      gamma[i] = in[cursor++];

    for (int i = 0; i < beta.size(); i++)
      beta[i] = in[cursor++];

    return true;
  }
};
