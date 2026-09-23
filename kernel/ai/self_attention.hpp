#pragma once

#include "../containers/vector.hpp"
#include "../random/random.hpp"
#include "../utils/math_utils.hpp"

#include <cmath>

class SelfAttention {
private:
  static constexpr int INPUT_DIMENSION = 128;
  static constexpr int ATTENTION_DIMENSION = 128;

  // Same reasoning as IntentClassifier: without decay, these weights
  // (and therefore attention scores) can grow unbounded over many
  // epochs, and exp() in the softmax below will eventually overflow.
  static constexpr float WEIGHT_DECAY = 0.01f;

  // Clamping the scaled scores keeps exp() safely inside float32 range
  // no matter how large the weights get, as a hard backstop alongside
  // decay.
  static constexpr float MAX_SCORE_MAGNITUDE = 30.0f;

  // Max L2 norm allowed for any single gradient vector before it's
  // applied. Without this, a single unlucky step can push weights far
  // enough that logits/scores saturate on every future example (the
  // "everything is -0 or 9.21034 forever" failure mode) or overflow
  // outright. Weight decay alone is a gentle pull, not a hard limit -
  // this is the hard limit.
  static constexpr float GRADIENT_CLIP = 1.0f;

  Vector<float> query_weights;
  Vector<float> key_weights;
  Vector<float> value_weights;

  Vector<float> query;
  Vector<float> key;
  Vector<float> value;

  Vector<float> d_query_weights;
  Vector<float> d_key_weights;
  Vector<float> d_value_weights;

  void multiply(const Vector<float> &input, const Vector<float> &weights,
                Vector<float> &output) {
    output.clear();

    for (int i = 0; i < ATTENTION_DIMENSION; i++) {
      float sum = 0.0f;

      for (int j = 0; j < INPUT_DIMENSION; j++) {
        int weight_index = j * ATTENTION_DIMENSION + i;

        sum += input[j] * weights[weight_index];
      }

      output.push_back(sum);
    }
  }

  Vector<float> backward_projection(const Vector<float> &input,
                                    const Vector<float> &weights,
                                    const Vector<float> &d_output,
                                    Vector<float> &d_weights_accum) {

    Vector<float> d_input;
    d_input.resize(INPUT_DIMENSION, 0.0f);

    for (int i = 0; i < ATTENTION_DIMENSION; i++) {
      for (int j = 0; j < INPUT_DIMENSION; j++) {
        int weight_index = j * ATTENTION_DIMENSION + i;

        d_weights_accum[weight_index] += input[j] * d_output[i];

        d_input[j] += d_output[i] * weights[weight_index];
      }
    }

    return d_input;
  }

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
  SelfAttention() {
    constexpr int WEIGHT_COUNT = INPUT_DIMENSION * ATTENTION_DIMENSION;

    query_weights.reserve(WEIGHT_COUNT);
    key_weights.reserve(WEIGHT_COUNT);
    value_weights.reserve(WEIGHT_COUNT);

    for (int i = 0; i < WEIGHT_COUNT; i++) {
      query_weights.push_back(
          (static_cast<float>(Random::next() % 2001) - 1000.0f) / 10000.0f);

      key_weights.push_back(
          (static_cast<float>(Random::next() % 2001) - 1000.0f) / 10000.0f);

      value_weights.push_back(
          (static_cast<float>(Random::next() % 2001) - 1000.0f) / 10000.0f);
    }

    d_query_weights.resize(WEIGHT_COUNT, 0.0f);
    d_key_weights.resize(WEIGHT_COUNT, 0.0f);
    d_value_weights.resize(WEIGHT_COUNT, 0.0f);
  }

  void build(const Vector<float> &input) {
    query.clear();
    key.clear();
    value.clear();

    multiply(input, query_weights, query);
    multiply(input, key_weights, key);
    multiply(input, value_weights, value);
  }

  Vector<float> &get_query() { return query; }

  Vector<float> &get_key() { return key; }

  Vector<float> &get_value() { return value; }

  Vector<Vector<float>> attention_scores;
  Vector<Vector<float>> attention_weights;

  // Direction each score was clamped in during the forward pass:
  //   0.0  -> not clamped, gradient flows through untouched
  //  +1.0  -> clamped at the HIGH boundary (MAX_SCORE_MAGNITUDE)
  //  -1.0  -> clamped at the LOW boundary (-MAX_SCORE_MAGNITUDE)
  //
  // Same reasoning as IntentClassifier::cached_clip_direction: a plain
  // binary mask blocks corrective gradient along with runaway gradient,
  // which can permanently freeze an attention score once it saturates.
  // Storing direction lets backward() allow gradient that pulls a
  // saturated score back into range, while still blocking gradient that
  // would push it further past the boundary.
  Vector<Vector<float>> score_clip_direction;

  float dot_product(const Vector<float> &a, const Vector<float> &b) {
    float sum = 0.0f;

    for (int i = 0; i < ATTENTION_DIMENSION; i++)
      sum += a[i] * b[i];

    return sum;
  }

  void calculate_attention_scores(const Vector<Vector<float>> &queries,
                                  const Vector<Vector<float>> &keys) {

    attention_scores.clear();
    score_clip_direction.clear();

    for (int i = 0; i < queries.size(); i++) {
      Vector<float> scores;
      Vector<float> direction_row;

      for (int j = 0; j < keys.size(); j++) {
        float score = dot_product(queries[i], keys[j]);

        score /= MathUtils::sqrt(128.0f);

        // NaN-safety: a non-finite score used to fall through to the
        // "else" branch below (both ">" and "<" comparisons against
        // NaN are false), so it was treated as perfectly normal and
        // passed unclamped into the softmax. Treat it the same as a
        // high-side saturation: clamp it and mark direction so
        // corrective gradient can still pull it back down.
        if (!std::isfinite(score)) {
          score = MAX_SCORE_MAGNITUDE;
          direction_row.push_back(1.0f);
        } else if (score > MAX_SCORE_MAGNITUDE) {
          score = MAX_SCORE_MAGNITUDE;
          direction_row.push_back(1.0f);
        } else if (score < -MAX_SCORE_MAGNITUDE) {
          score = -MAX_SCORE_MAGNITUDE;
          direction_row.push_back(-1.0f);
        } else {
          direction_row.push_back(0.0f);
        }

        scores.push_back(score);
      }

      attention_scores.push_back(scores);
      score_clip_direction.push_back(direction_row);
    }
  }

  void calculate_attention_weights() {
    attention_weights.clear();

    for (int i = 0; i < attention_scores.size(); i++) {
      Vector<float> weights;

      float max_score = attention_scores[i][0];

      for (int j = 1; j < attention_scores[i].size(); j++) {

        if (attention_scores[i][j] > max_score)
          max_score = attention_scores[i][j];
      }

      float sum = 0.0f;

      for (int j = 0; j < attention_scores[i].size(); j++) {

        float value = MathUtils::exp(attention_scores[i][j] - max_score);

        weights.push_back(value);
        sum += value;
      }

      for (int j = 0; j < weights.size(); j++) {

        weights[j] /= sum;
      }

      attention_weights.push_back(weights);
    }
  }

  Vector<Vector<float>> attention_output;

  void calculate_attention_output(const Vector<Vector<float>> &values) {

    attention_output.clear();

    for (int i = 0; i < attention_weights.size(); i++) {

      Vector<float> output;

      for (int dimension = 0; dimension < ATTENTION_DIMENSION; dimension++) {

        float sum = 0.0f;

        for (int j = 0; j < values.size(); j++) {

          sum += attention_weights[i][j] * values[j][dimension];
        }

        output.push_back(sum);
      }

      attention_output.push_back(output);
    }
  }

  Vector<Vector<float>>
  backward(const Vector<Vector<float>> &d_attention_output,
           const Vector<Vector<float>> &queries,
           const Vector<Vector<float>> &keys,
           const Vector<Vector<float>> &values,
           const Vector<Vector<float>> &inputs) {

    int seq_len = d_attention_output.size();

    Vector<Vector<float>> d_weights_matrix;

    for (int i = 0; i < seq_len; i++) {
      Vector<float> row;
      row.resize(seq_len, 0.0f);

      for (int j = 0; j < seq_len; j++) {
        float dot = 0.0f;

        for (int dim = 0; dim < ATTENTION_DIMENSION; dim++) {

          dot += d_attention_output[i][dim] * values[j][dim];
        }

        row[j] = dot;
      }

      d_weights_matrix.push_back(row);
    }

    Vector<Vector<float>> d_values;

    for (int j = 0; j < seq_len; j++) {
      Vector<float> vec;
      vec.resize(ATTENTION_DIMENSION, 0.0f);

      d_values.push_back(vec);
    }

    for (int i = 0; i < seq_len; i++) {
      for (int j = 0; j < seq_len; j++) {
        for (int dim = 0; dim < ATTENTION_DIMENSION; dim++) {

          d_values[j][dim] +=
              attention_weights[i][j] * d_attention_output[i][dim];
        }
      }
    }

    Vector<Vector<float>> d_scores;

    for (int i = 0; i < seq_len; i++) {
      float dot_sum = 0.0f;

      for (int j = 0; j < seq_len; j++) {
        dot_sum += attention_weights[i][j] * d_weights_matrix[i][j];
      }

      Vector<float> row;
      row.resize(seq_len, 0.0f);

      // Direction-aware straight-through estimator: block gradient only
      // when it would push an already-saturated score further past the
      // boundary it hit; allow gradient that pulls it back into range.
      // See score_clip_direction and IntentClassifier::backward for the
      // full reasoning - a plain binary mask here permanently freezes
      // any attention pair that ever saturates.
      for (int j = 0; j < seq_len; j++) {
        float raw = attention_weights[i][j] * (d_weights_matrix[i][j] - dot_sum);
        float direction = score_clip_direction[i][j];

        if (direction > 0.0f && raw < 0.0f)
          raw = 0.0f;
        else if (direction < 0.0f && raw > 0.0f)
          raw = 0.0f;

        row[j] = raw;
      }

      d_scores.push_back(row);
    }

    float scale =
        1.0f / MathUtils::sqrt(static_cast<float>(ATTENTION_DIMENSION));

    Vector<Vector<float>> d_queries;

    for (int i = 0; i < seq_len; i++) {
      Vector<float> vec;
      vec.resize(ATTENTION_DIMENSION, 0.0f);

      d_queries.push_back(vec);
    }

    Vector<Vector<float>> d_keys;

    for (int j = 0; j < seq_len; j++) {
      Vector<float> vec;
      vec.resize(ATTENTION_DIMENSION, 0.0f);

      d_keys.push_back(vec);
    }

    for (int i = 0; i < seq_len; i++) {
      for (int j = 0; j < seq_len; j++) {
        float g = d_scores[i][j] * scale;

        for (int dim = 0; dim < ATTENTION_DIMENSION; dim++) {

          d_queries[i][dim] += g * keys[j][dim];

          d_keys[j][dim] += g * queries[i][dim];
        }
      }
    }

    Vector<Vector<float>> d_input;

    for (int i = 0; i < seq_len; i++) {
      Vector<float> d_in_q = backward_projection(inputs[i], query_weights,
                                                 d_queries[i], d_query_weights);

      Vector<float> d_in_k =
          backward_projection(inputs[i], key_weights, d_keys[i], d_key_weights);

      Vector<float> d_in_v = backward_projection(inputs[i], value_weights,
                                                 d_values[i], d_value_weights);

      Vector<float> d_in;
      d_in.resize(INPUT_DIMENSION, 0.0f);

      for (int k = 0; k < INPUT_DIMENSION; k++) {

        d_in[k] = d_in_q[k] + d_in_k[k] + d_in_v[k];
      }

      d_input.push_back(d_in);
    }

    return d_input;
  }

  // Pushes pointers to this module's own gradient vectors into out, so
  // Trainer can include them when computing one combined L2 norm across
  // the WHOLE network's gradients (embedding + every layer + classifier)
  // and scale everything down together if that combined norm is too
  // large. Per-vector clipping in apply_gradients() below only bounds
  // each individual vector in isolation - it can't catch the case where
  // many vectors are each fine on their own but their combined effect
  // on the network still pushes things too far in one step.
  void collect_gradients(Vector<Vector<float> *> &out) {
    out.push_back(&d_query_weights);
    out.push_back(&d_key_weights);
    out.push_back(&d_value_weights);
  }

  void zero_grad() {
    for (int i = 0; i < d_query_weights.size(); i++) {

      d_query_weights[i] = 0.0f;
    }

    for (int i = 0; i < d_key_weights.size(); i++) {

      d_key_weights[i] = 0.0f;
    }

    for (int i = 0; i < d_value_weights.size(); i++) {

      d_value_weights[i] = 0.0f;
    }
  }

  void apply_gradients(float learning_rate) {
    // Bound each gradient's L2 norm before it ever touches the weights.
    // This is what was missing before: WEIGHT_DECAY below only pulls
    // weights gently toward zero every step, it does not cap how much
    // a single bad/large gradient can push them in one step.
    clip_gradient(d_query_weights, GRADIENT_CLIP);
    clip_gradient(d_key_weights, GRADIENT_CLIP);
    clip_gradient(d_value_weights, GRADIENT_CLIP);

    // Same decoupled-in-formula L2 decay pattern as IntentClassifier.
    for (int i = 0; i < query_weights.size(); i++) {
      query_weights[i] -=
          learning_rate * (d_query_weights[i] + WEIGHT_DECAY * query_weights[i]);
    }

    for (int i = 0; i < key_weights.size(); i++) {
      key_weights[i] -=
          learning_rate * (d_key_weights[i] + WEIGHT_DECAY * key_weights[i]);
    }

    for (int i = 0; i < value_weights.size(); i++) {
      value_weights[i] -=
          learning_rate * (d_value_weights[i] + WEIGHT_DECAY * value_weights[i]);
    }
  }

  void update_weights(const Vector<float> &query_gradients,
                      const Vector<float> &key_gradients,
                      const Vector<float> &value_gradients,
                      float learning_rate) {

    for (int i = 0; i < query_weights.size(); i++) {

      query_weights[i] -= learning_rate * query_gradients[i];
    }

    for (int i = 0; i < key_weights.size(); i++) {

      key_weights[i] -= learning_rate * key_gradients[i];
    }

    for (int i = 0; i < value_weights.size(); i++) {

      value_weights[i] -= learning_rate * value_gradients[i];
    }
  }

  int parameter_count() const {
    return query_weights.size() + key_weights.size() + value_weights.size();
  }

  void export_weights(Vector<float> &out) const {
    for (int i = 0; i < query_weights.size(); i++)
      out.push_back(query_weights[i]);

    for (int i = 0; i < key_weights.size(); i++)
      out.push_back(key_weights[i]);

    for (int i = 0; i < value_weights.size(); i++)
      out.push_back(value_weights[i]);
  }

  bool import_weights(const Vector<float> &in, int &cursor) {
    int count = parameter_count();

    if (cursor + count > in.size())
      return false;

    for (int i = 0; i < query_weights.size(); i++)
      query_weights[i] = in[cursor++];

    for (int i = 0; i < key_weights.size(); i++)
      key_weights[i] = in[cursor++];

    for (int i = 0; i < value_weights.size(); i++)
      value_weights[i] = in[cursor++];

    return true;
  }
};
