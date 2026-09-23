#pragma once

#include "../containers/vector.hpp"
#include "../utils/math_utils.hpp"

#include <cmath>

class LayerNorm {
private:
  static constexpr int DIMENSION = 128;

  Vector<float> gamma;
  Vector<float> beta;

  float epsilon = 0.0001f;

  Vector<float> d_gamma;
  Vector<float> d_beta;

  static constexpr float GRADIENT_CLIP = 1.0f;
  static constexpr float WEIGHT_DECAY = 0.01f;

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
      float diff = input[i] - mean;
      squared_diff_sum += diff * diff;
    }

    float variance = squared_diff_sum / DIMENSION;
    float invstd = 1.0f / MathUtils::sqrt(variance + epsilon);

    for (int i = 0; i < DIMENSION; i++) {
      float xhat = (input[i] - mean) * invstd;

      output[i] = xhat * gamma[i] + beta[i];
    }

    return output;
  }

  // dout is the gradient of the loss w.r.t. this layer's output.
  // input MUST be the exact same vector that was passed into normalize()
  // during the forward pass.
  Vector<float> backward(const Vector<float> &dout,
                         const Vector<float> &input) {
    int N = input.size();

    float mean = input.sum() / N;

    float squared_diff_sum = 0.f;

    for (int j = 0; j < N; j++) {
      // FIX: was MathUtils::pow(input[j] - mean, 2.f). If MathUtils::pow
      // is a generic exp(exponent * log(base)) implementation it returns
      // NaN for any negative base - and (input[j] - mean) is negative
      // roughly half the time, every call. That NaN poisoned variance /
      // invstd / xhat / dxhat / dx here, which then poisoned every
      // gradient upstream of this layer. Trainer::clip_global_norm sums
      // squared norms across the WHOLE network before checking
      // isfinite(), so one NaN here zeroed every gradient in the entire
      // model for that step (only weight decay still applied), which is
      // what stalled training at the cross-entropy floor
      // (-log(0.0001) == 9.21034...) forever. normalize() above was
      // already fixed for this; this was the one spot that wasn't.
      float diff = input[j] - mean;
      squared_diff_sum += diff * diff;
    }

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

    Vector<float> dx;
    dx.resize(N, 0.0f);

    for (int i = 0; i < N; i++) {
      dx[i] =
          (invstd / N) * (N * dxhat[i] - sum_dxhat - xhat[i] * sum_dxhat_xhat);
    }

    return dx;
  }

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
    clip_gradient(d_gamma, GRADIENT_CLIP);
    clip_gradient(d_beta, GRADIENT_CLIP);

    for (int i = 0; i < gamma.size(); i++)
      gamma[i] -= learning_rate * (d_gamma[i] + WEIGHT_DECAY * gamma[i]);

    for (int i = 0; i < beta.size(); i++)
      beta[i] -= learning_rate * d_beta[i];
  }

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
