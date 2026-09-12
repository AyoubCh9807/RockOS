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

  void zero_grad() {
    for (int i = 0; i < d_gamma.size(); i++)
      d_gamma[i] = 0.f;
    for (int i = 0; i < d_beta.size(); i++)
      d_beta[i] = 0.f;
  }

  void apply_gradients(float learning_rate) {
    for (int i = 0; i < gamma.size(); i++)
      gamma[i] -= learning_rate * d_gamma[i];

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
