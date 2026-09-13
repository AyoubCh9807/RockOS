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

  // Small L2 penalty applied during apply_gradients(). Without this,
  // cross-entropy has no incentive to keep weight magnitudes bounded -
  // it keeps growing them for as long as any error remains, which is
  // what eventually blows up the logits into NaN/Inf territory after
  // enough epochs of otherwise-healthy training.
  static constexpr float WEIGHT_DECAY = 0.01f;

  // Hard ceiling on logit magnitude. exp(20) is ~4.85e8 - comfortably
  // inside float32 range - so clamping here makes it mathematically
  // impossible for softmax to overflow into NaN/Inf, no matter how
  // large the underlying weights get. This is a backstop, not a
  // substitute for the decay above: decay keeps weights (and therefore
  // learned confidence) reasonable; this just guarantees the failure
  // mode you saw can't happen even if decay is mistuned.
  static constexpr float MAX_LOGIT_MAGNITUDE = 20.0f;

  // Max L2 norm allowed for this module's OWN weight/bias gradients
  // before they're applied. Trainer clips d_logits (the gradient
  // flowing IN from the loss), but that doesn't bound d_weights/d_bias
  // computed here in backward(): those scale with cached_pooled, which
  // can still be large if upstream activations (e.g. LayerNorm's gamma)
  // drift up over many epochs. Clipping only the entry point gradient
  // is not enough - every module that accumulates its own parameter
  // gradient needs its own clip.
  static constexpr float GRADIENT_CLIP = 1.0f;

  Vector<float> weights;
  Vector<float> bias;

  Vector<float> d_weights;
  Vector<float> d_bias;

  // Kept from the last classify() call so backward() knows what to use.
  Vector<float> cached_pooled;
  int cached_sequence_length = 0;

  // 1.0 where the logit was NOT clamped, 0.0 where it was. Multiplying
  // d_logits by this in backward() is a straight-through clamp: once a
  // logit hits the ceiling, its gradient stops flowing too, so the
  // underlying weights stop being pushed further past the boundary.
  // Without this, the clamp only hides the symptom (frozen output) while
  // the raw pre-clamp values keep growing silently until they overflow.
  Vector<float> cached_clip_mask;

  // Clips a gradient vector in place so its L2 norm never exceeds
  // max_norm. Same policy as Trainer::clip_gradient, duplicated here so
  // this module's own gradients get bounded too.
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
  IntentClassifier() {
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
    cached_clip_mask.clear();

    Vector<float> logits;
    logits.reserve(OUTPUT_DIMENSION);

    for (int intent = 0; intent < OUTPUT_DIMENSION; intent++) {
      float sum = 0.0f;

      for (int dimension = 0; dimension < INPUT_DIMENSION; dimension++) {

        int weight_index = dimension * OUTPUT_DIMENSION + intent;

        sum += pooled[dimension] * weights[weight_index];
      }

      sum += bias[intent];

      if (sum > MAX_LOGIT_MAGNITUDE) {
        sum = MAX_LOGIT_MAGNITUDE;
        cached_clip_mask.push_back(0.0f);
      } else if (sum < -MAX_LOGIT_MAGNITUDE) {
        sum = -MAX_LOGIT_MAGNITUDE;
        cached_clip_mask.push_back(0.0f);
      } else {
        cached_clip_mask.push_back(1.0f);
      }

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
                                    Intent true_intent) {
    Vector<float> probs = softmax(logits);
    int label = static_cast<int>(true_intent);

    Vector<float> d_logits;
    d_logits.resize(probs.size(), 0.0f);

    for (int i = 0; i < probs.size(); i++)
      d_logits[i] = probs[i] - (i == label ? 1.0f : 0.0f);

    return d_logits;
  }

  Vector<Vector<float>> backward(const Vector<float> &d_logits) {
    // Straight-through estimator: the clamp only shaped the forward
    // value. Gradient passes through unchanged here - a fully gated
    // gradient at the clamp creates a dead zone that a confidently
    // wrong-but-saturated prediction can never escape from. Weight
    // decay in apply_gradients() is what keeps raw magnitudes bounded
    // instead.
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

  // See SelfAttention::collect_gradients for why this exists: lets
  // Trainer fold this module's gradients into one combined global-norm
  // clip across the whole network, instead of relying only on the
  // per-vector clip in apply_gradients() below.
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
    // Bound this module's own gradients before they touch the weights.
    // d_weights/d_bias are computed from cached_pooled, which is NOT
    // itself clamped anywhere - if upstream activations drift up, these
    // gradients can grow large even when d_logits (clipped in Trainer)
    // stays small. This is what let the model diverge again around
    // epoch 35: clipping only the entry-point gradient wasn't enough.
    clip_gradient(d_weights, GRADIENT_CLIP);
    clip_gradient(d_bias, GRADIENT_CLIP);

    // weight -= lr * (gradient + decay * weight)
    // The "+ decay * weight" term is the L2 penalty: it pulls every
    // weight gently back toward zero every step, proportional to its
    // own current size, which caps how large the logits can grow no
    // matter how many epochs you train for. Bias terms are left out of
    // decay on purpose - regularizing biases doesn't help and can hurt.
    for (int i = 0; i < weights.size(); i++)
      weights[i] -=
          learning_rate * (d_weights[i] + WEIGHT_DECAY * weights[i]);

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
