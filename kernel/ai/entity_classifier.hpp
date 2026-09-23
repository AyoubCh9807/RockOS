#pragma once

#include "../containers/vector.hpp"
#include "../random/random.hpp"
#include "../utils/math_utils.hpp"
#include "intent_classifier.hpp"

#include <cmath>

// Second prediction head over the same pooled transformer output
// IntentClassifier reads. Predicts *which entity* (app/window) a command
// refers to, independently of intent. Kept as its own class rather than
// folded into IntentClassifier so the two heads have fully independent
// weights/gradients/loss - Trainer combines them at the trunk boundary
// (see Trainer::train_on_example).
class EntityClassifier {
public:
  using Entity = IntentClassifier::Entity;

private:
  static constexpr int INPUT_DIMENSION = 128;
  static constexpr int OUTPUT_DIMENSION = static_cast<int>(Entity::COUNT);

  static constexpr float WEIGHT_DECAY = 0.01f;
  static constexpr float MAX_LOGIT_MAGNITUDE = 20.0f;
  static constexpr float GRADIENT_CLIP = 1.0f;

  Vector<float> weights;
  Vector<float> bias;

  Vector<float> d_weights;
  Vector<float> d_bias;

  Vector<float> cached_pooled;
  int cached_sequence_length = 0;

  Vector<float> cached_clip_direction;

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
  EntityClassifier() {
    weights.reserve(INPUT_DIMENSION * OUTPUT_DIMENSION);
    bias.reserve(OUTPUT_DIMENSION);

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

    cached_pooled = pooled;
    cached_sequence_length = sequence.size();
    cached_clip_direction.clear();

    Vector<float> logits;
    logits.reserve(OUTPUT_DIMENSION);

    for (int entity = 0; entity < OUTPUT_DIMENSION; entity++) {
      float sum = 0.0f;

      for (int dimension = 0; dimension < INPUT_DIMENSION; dimension++) {

        int weight_index = dimension * OUTPUT_DIMENSION + entity;

        sum += pooled[dimension] * weights[weight_index];
      }

      sum += bias[entity];

      if (!std::isfinite(sum)) {
        sum = MAX_LOGIT_MAGNITUDE;
        cached_clip_direction.push_back(1.0f);
      } else if (sum > MAX_LOGIT_MAGNITUDE) {
        sum = MAX_LOGIT_MAGNITUDE;
        cached_clip_direction.push_back(1.0f);
      } else if (sum < -MAX_LOGIT_MAGNITUDE) {
        sum = -MAX_LOGIT_MAGNITUDE;
        cached_clip_direction.push_back(-1.0f);
      } else {
        cached_clip_direction.push_back(0.0f);
      }

      logits.push_back(sum);
    }

    return logits;
  }

  Entity predict(const Vector<Vector<float>> &sequence) {
    Vector<float> logits = classify(sequence);

    if (logits.size() == 0)
      return Entity::NONE;

    int best_index = 0;

    for (int i = 1; i < logits.size(); i++) {
      if (logits[i] > logits[best_index])
        best_index = i;
    }

    return static_cast<Entity>(best_index);
  }

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

  Vector<float> gradient_from_label(const Vector<float> &logits,
                                    Entity true_entity) {
    Vector<float> probs = softmax(logits);
    int label = static_cast<int>(true_entity);

    Vector<float> d_logits;
    d_logits.resize(probs.size(), 0.0f);

    for (int i = 0; i < probs.size(); i++)
      d_logits[i] = probs[i] - (i == label ? 1.0f : 0.0f);

    return d_logits;
  }

  Vector<Vector<float>> backward(const Vector<float> &d_logits) {
    Vector<float> gated;
    gated.resize(OUTPUT_DIMENSION, 0.0f);

    for (int entity = 0; entity < OUTPUT_DIMENSION; entity++) {
      float direction = cached_clip_direction[entity];
      float g = d_logits[entity];

      if (direction > 0.0f && g < 0.0f)
        g = 0.0f;
      else if (direction < 0.0f && g > 0.0f)
        g = 0.0f;

      gated[entity] = g;

      d_bias[entity] += g;

      for (int dimension = 0; dimension < INPUT_DIMENSION; dimension++) {
        int weight_index = dimension * OUTPUT_DIMENSION + entity;
        d_weights[weight_index] += cached_pooled[dimension] * g;
      }
    }

    Vector<float> d_pooled;
    d_pooled.resize(INPUT_DIMENSION, 0.0f);

    for (int dimension = 0; dimension < INPUT_DIMENSION; dimension++) {
      float sum = 0.f;
      for (int entity = 0; entity < OUTPUT_DIMENSION; entity++) {
        int weight_index = dimension * OUTPUT_DIMENSION + entity;
        sum += gated[entity] * weights[weight_index];
      }
      d_pooled[dimension] = sum;
    }

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

  void collect_gradients(Vector<Vector<float> *> &out) {
    out.push_back(&d_weights);
    out.push_back(&d_bias);
  }

  void zero_grad() {
    for (int i = 0; i < d_weights.size(); i++)
      d_weights[i] = 0.f;
    for (int i = 0; i < d_bias.size(); i++)
      d_bias[i] = 0.f;
  }

  void apply_gradients(float learning_rate) {
    clip_gradient(d_weights, GRADIENT_CLIP);
    clip_gradient(d_bias, GRADIENT_CLIP);

    for (int i = 0; i < weights.size(); i++)
      weights[i] -= learning_rate * (d_weights[i] + WEIGHT_DECAY * weights[i]);

    for (int i = 0; i < bias.size(); i++)
      bias[i] -= learning_rate * d_bias[i];
  }

  void update_weights(const Vector<float> &weight_gradients,
                      const Vector<float> &bias_gradients,
                      float learning_rate) {

    for (int i = 0; i < weights.size(); i++)
      weights[i] -= learning_rate * weight_gradients[i];

    for (int i = 0; i < bias.size(); i++)
      bias[i] -= learning_rate * bias_gradients[i];
  }

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
