#pragma once

#include "../containers/vector.hpp"
#include "../random/random.hpp"
#include "../utils/math_utils.hpp"

class IntentClassifier {
public:
  enum class Intent {
    UNKNOWN,

    GREETING,
    MEMORY_USAGE,
    TIME,
    UPTIME,

    OPEN_APP,
    CLOSE_WINDOW,
    MINIMIZE_WINDOW,

    HELP,

    COUNT
  };

private:
  static constexpr int INPUT_DIMENSION = 128;
  static constexpr int OUTPUT_DIMENSION = static_cast<int>(Intent::COUNT);

  Vector<float> weights;
  Vector<float> bias;

  Vector<float> d_weights;
  Vector<float> d_bias;

  // Kept from the last classify() call so backward() knows what to use.
  Vector<float> cached_pooled;
  int cached_sequence_length = 0;

public:
  IntentClassifier() {
    weights.reserve(INPUT_DIMENSION * OUTPUT_DIMENSION);
    bias.reserve(OUTPUT_DIMENSION);

    // NOTE: originally these started at all-zero. That means every intent
    // gets an identical starting logit, so the very first gradient step
    // is the only thing breaking the symmetry between classes - training
    // works much better if we start from small random values instead
    // (same trick FeedForwardNetwork already uses).
    for (int i = 0; i < INPUT_DIMENSION * OUTPUT_DIMENSION; i++)
      weights.push_back((static_cast<float>(Random::next() % 2001) - 1000.0f) /
                        10000.0f);

    for (int i = 0; i < OUTPUT_DIMENSION; i++)
      bias.push_back(0.0f);

    d_weights.resize(INPUT_DIMENSION * OUTPUT_DIMENSION, 0.0f);
    d_bias.resize(OUTPUT_DIMENSION, 0.0f);
  }

  Vector<float> mean_pool(const Vector<Vector<float>> &sequence) {
    Vector<float> pooled;
    pooled.reserve(INPUT_DIMENSION);

    if (sequence.size() == 0)
      return pooled;

    for (int dimension = 0; dimension < INPUT_DIMENSION; dimension++) {

      float sum = 0.0f;

      for (int token = 0; token < sequence.size(); token++)
        sum += sequence[token][dimension];

      pooled.push_back(sum / sequence.size());
    }

    return pooled;
  }

  Vector<float> classify(const Vector<Vector<float>> &sequence) {
    Vector<float> pooled = mean_pool(sequence);

    // Remember these for backward().
    cached_pooled = pooled;
    cached_sequence_length = sequence.size();

    Vector<float> logits;
    logits.reserve(OUTPUT_DIMENSION);

    for (int intent = 0; intent < OUTPUT_DIMENSION; intent++) {
      float sum = 0.0f;

      for (int dimension = 0; dimension < INPUT_DIMENSION; dimension++) {

        int weight_index = dimension * OUTPUT_DIMENSION + intent;

        sum += pooled[dimension] * weights[weight_index];
      }

      sum += bias[intent];

      logits.push_back(sum);
    }

    return logits;
  }

  Intent predict(const Vector<Vector<float>> &sequence) {
    Vector<float> logits = classify(sequence);

    if (logits.size() == 0)
      return Intent::UNKNOWN;

    int best_index = 0;

    for (int i = 1; i < logits.size(); i++) {
      if (logits[i] > logits[best_index])
        best_index = i;
    }

    return static_cast<Intent>(best_index);
  }

  // Turns raw logits into probabilities that add up to 1.
  Vector<float> softmax(const Vector<float> &logits) {
    Vector<float> probs;
    probs.resize(logits.size(), 0.0f);

    float max_logit = logits[0];
    for (int i = 1; i < logits.size(); i++)
      if (logits[i] > max_logit)
        max_logit = logits[i];

    float sum = 0.0f;
    for (int i = 0; i < logits.size(); i++) {
      probs[i] = MathUtils::exp(logits[i] - max_logit);
      sum += probs[i];
    }

    for (int i = 0; i < logits.size(); i++)
      probs[i] /= sum;

    return probs;
  }

  // Gradient of softmax cross-entropy loss w.r.t. the logits. This has a
  // famously simple form: (predicted probabilities) - (the correct answer
  // as a one-hot vector).
  Vector<float> gradient_from_label(const Vector<float> &logits,
                                    Intent true_intent) {
    Vector<float> probs = softmax(logits);
    int label = static_cast<int>(true_intent);

    Vector<float> d_logits;
    d_logits.resize(probs.size(), 0.0f);

    for (int i = 0; i < probs.size(); i++)
      d_logits[i] = probs[i] - (i == label ? 1.0f : 0.0f);

    return d_logits;
  }

  // Backpropagation 

  // d_logits should come from gradient_from_label(). Must be called right
  // after classify() ran on the sequence this gradient corresponds to.
  //
  // Returns the gradient w.r.t. every token in the sequence (mean-pooling
  // splits the gradient equally across all of them), and accumulates
  // weight/bias gradients internally.
  Vector<Vector<float>> backward(const Vector<float> &d_logits) {
    for (int intent = 0; intent < OUTPUT_DIMENSION; intent++) {
      d_bias[intent] += d_logits[intent];

      for (int dimension = 0; dimension < INPUT_DIMENSION; dimension++) {
        int weight_index = dimension * OUTPUT_DIMENSION + intent;
        d_weights[weight_index] += cached_pooled[dimension] * d_logits[intent];
      }
    }

    Vector<float> d_pooled;
    d_pooled.resize(INPUT_DIMENSION, 0.0f);

    for (int dimension = 0; dimension < INPUT_DIMENSION; dimension++) {
      float sum = 0.f;
      for (int intent = 0; intent < OUTPUT_DIMENSION; intent++) {
        int weight_index = dimension * OUTPUT_DIMENSION + intent;
        sum += d_logits[intent] * weights[weight_index];
      }
      d_pooled[dimension] = sum;
    }

    // mean_pool averaged over every token, so the gradient splits equally
    // and flows back into each token's vector.
    Vector<Vector<float>> d_sequence;
    float scale =
        cached_sequence_length > 0 ? 1.0f / cached_sequence_length : 0.0f;

    for (int t = 0; t < cached_sequence_length; t++) {
      Vector<float> d_token;
      d_token.resize(INPUT_DIMENSION, 0.0f);

      for (int dimension = 0; dimension < INPUT_DIMENSION; dimension++)
        d_token[dimension] = d_pooled[dimension] * scale;

      d_sequence.push_back(d_token);
    }

    return d_sequence;
  }

  void zero_grad() {
    for (int i = 0; i < d_weights.size(); i++)
      d_weights[i] = 0.f;
    for (int i = 0; i < d_bias.size(); i++)
      d_bias[i] = 0.f;
  }

  void apply_gradients(float learning_rate) {
    for (int i = 0; i < weights.size(); i++)
      weights[i] -= learning_rate * d_weights[i];

    for (int i = 0; i < bias.size(); i++)
      bias[i] -= learning_rate * d_bias[i];
  }

  // Kept for backwards compatibility.
  void update_weights(const Vector<float> &weight_gradients,
                      const Vector<float> &bias_gradients,
                      float learning_rate) {

    for (int i = 0; i < weights.size(); i++)
      weights[i] -= learning_rate * weight_gradients[i];

    for (int i = 0; i < bias.size(); i++)
      bias[i] -= learning_rate * bias_gradients[i];
  }

  // Weight serialization 
  int parameter_count() const { return weights.size() + bias.size(); }

  void export_weights(Vector<float> &out) const {
    for (int i = 0; i < weights.size(); i++)
      out.push_back(weights[i]);
    for (int i = 0; i < bias.size(); i++)
      out.push_back(bias[i]);
  }

  bool import_weights(const Vector<float> &in, int &cursor) {
    if (cursor + parameter_count() > in.size())
      return false;

    for (int i = 0; i < weights.size(); i++)
      weights[i] = in[cursor++];

    for (int i = 0; i < bias.size(); i++)
      bias[i] = in[cursor++];

    return true;
  }
};
