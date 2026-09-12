#pragma once

#include "../containers/vector.hpp"
#include "../random/random.hpp"

class FeedForwardNetwork {
private:
  static constexpr int INPUT_DIMENSION = 128;
  static constexpr int HIDDEN_DIMENSION = 256;
  static constexpr int OUTPUT_DIMENSION = 128;

  Vector<float> first_weights;
  Vector<float> first_bias;

  Vector<float> second_weights;
  Vector<float> second_bias;

  // Gradients, same shapes as the weights/biases above.
  Vector<float> d_first_weights;
  Vector<float> d_first_bias;
  Vector<float> d_second_weights;
  Vector<float> d_second_bias;

  float relu(float value) { return value > 0.0f ? value : 0.0f; }

public:
  FeedForwardNetwork() {
    first_weights.reserve(INPUT_DIMENSION * HIDDEN_DIMENSION);
    first_bias.reserve(HIDDEN_DIMENSION);

    second_weights.reserve(HIDDEN_DIMENSION * OUTPUT_DIMENSION);
    second_bias.reserve(OUTPUT_DIMENSION);

    // Initialize every weight to a small random value.
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

  // Backpropagation 

  // dout is the gradient of the loss w.r.t. this layer's output.
  // input MUST be the same vector passed into forward() - the hidden
  // layer is recomputed from it here instead of being cached, so this
  // class stays safe to call once per token in a sequence.
  //
  // Returns the gradient w.r.t. this layer's input, and accumulates
  // weight/bias gradients internally.
  Vector<float> backward(const Vector<float> &dout,
                         const Vector<float> &input) {
    // Recompute the hidden layer (before and after ReLU) from scratch.
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

    // Backward through the second linear layer.
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

    // Backward through ReLU: gradient only flows where the pre-activation
    // value was positive.
    Vector<float> d_hidden_pre;
    d_hidden_pre.resize(HIDDEN_DIMENSION, 0.0f);

    for (int j = 0; j < HIDDEN_DIMENSION; j++)
      d_hidden_pre[j] = hidden_pre[j] > 0.0f ? d_hidden[j] : 0.0f;

    // Backward through the first linear layer.
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
    for (int i = 0; i < first_weights.size(); i++)
      first_weights[i] -= learning_rate * d_first_weights[i];

    for (int i = 0; i < first_bias.size(); i++)
      first_bias[i] -= learning_rate * d_first_bias[i];

    for (int i = 0; i < second_weights.size(); i++)
      second_weights[i] -= learning_rate * d_second_weights[i];

    for (int i = 0; i < second_bias.size(); i++)
      second_bias[i] -= learning_rate * d_second_bias[i];
  }

  // Kept for backwards compatibility.
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
