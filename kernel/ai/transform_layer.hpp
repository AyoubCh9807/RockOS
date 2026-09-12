#pragma once

#include "../containers/vector.hpp"

#include "feed_forward_network.hpp"
#include "layer_norm.hpp"
#include "self_attention.hpp"

class TransformerLayer {
private:
  static constexpr int DIMENSION = 128;

  SelfAttention attention;

  LayerNorm attention_norm;
  FeedForwardNetwork feed_forward;
  LayerNorm feed_forward_norm;

  // Everything below is filled in by forward() and read back by backward().
  // A layer's backward() must always be called right after its forward()
  // on the very same input.
  Vector<Vector<float>> cached_input;
  Vector<Vector<float>> cached_queries;
  Vector<Vector<float>> cached_keys;
  Vector<Vector<float>> cached_values;
  Vector<Vector<float>> cached_normalized_attention;
  Vector<Vector<float>> cached_ff_output;

  Vector<float> add_vectors(const Vector<float> &a, const Vector<float> &b) {
    Vector<float> result;
    result.resize(DIMENSION, 0.0f);

    for (int i = 0; i < DIMENSION; i++)
      result[i] = a[i] + b[i];

    return result;
  }

public:
  Vector<Vector<float>> forward(const Vector<Vector<float>> &input) {

    Vector<Vector<float>> queries;
    Vector<Vector<float>> keys;
    Vector<Vector<float>> values;

    for (int i = 0; i < input.size(); i++) {
      attention.build(input[i]);

      queries.push_back(attention.get_query());
      keys.push_back(attention.get_key());
      values.push_back(attention.get_value());
    }

    attention.calculate_attention_scores(queries, keys);

    attention.calculate_attention_weights();
    attention.calculate_attention_output(values);

    Vector<Vector<float>> normalized_attention;

    for (int i = 0; i < input.size(); i++) {
      Vector<float> residual =
          add_vectors(input[i], attention.attention_output[i]);

      normalized_attention.push_back(attention_norm.normalize(residual));
    }

    Vector<Vector<float>> ff_outputs;
    Vector<Vector<float>> output;

    for (int i = 0; i < normalized_attention.size(); i++) {

      Vector<float> ff_output = feed_forward.forward(normalized_attention[i]);

      ff_outputs.push_back(ff_output);

      Vector<float> residual = add_vectors(normalized_attention[i], ff_output);

      output.push_back(feed_forward_norm.normalize(residual));
    }

    cached_input = input;
    cached_queries = queries;
    cached_keys = keys;
    cached_values = values;
    cached_normalized_attention = normalized_attention;
    cached_ff_output = ff_outputs;

    return output;
  }

  Vector<Vector<float>> backward(const Vector<Vector<float>> &d_output) {

    int seq_len = d_output.size();

    Vector<Vector<float>> d_normalized_attention;

    for (int i = 0; i < seq_len; i++) {
      Vector<float> residual2 =
          add_vectors(cached_normalized_attention[i], cached_ff_output[i]);

      Vector<float> d_residual2 =
          feed_forward_norm.backward(d_output[i], residual2);

      Vector<float> d_norm1_from_ff =
          feed_forward.backward(d_residual2, cached_normalized_attention[i]);

      Vector<float> total_d_norm1;
      total_d_norm1.resize(DIMENSION, 0.0f);

      for (int k = 0; k < DIMENSION; k++) {
        total_d_norm1[k] = d_residual2[k] + d_norm1_from_ff[k];
      }

      d_normalized_attention.push_back(total_d_norm1);
    }

    Vector<Vector<float>> d_input_direct;
    Vector<Vector<float>> d_attention_output;

    for (int i = 0; i < seq_len; i++) {
      Vector<float> residual1 =
          add_vectors(cached_input[i], attention.attention_output[i]);

      Vector<float> d_residual1 =
          attention_norm.backward(d_normalized_attention[i], residual1);

      d_input_direct.push_back(d_residual1);

      d_attention_output.push_back(d_residual1);
    }

    Vector<Vector<float>> d_input_from_attention =
        attention.backward(d_attention_output, cached_queries, cached_keys,
                           cached_values, cached_input);

    Vector<Vector<float>> d_input;

    for (int i = 0; i < seq_len; i++) {
      Vector<float> total;
      total.resize(DIMENSION, 0.0f);

      for (int k = 0; k < DIMENSION; k++) {
        total[k] = d_input_direct[i][k] + d_input_from_attention[i][k];
      }

      d_input.push_back(total);
    }

    return d_input;
  }

  void zero_grad() {
    attention.zero_grad();
    attention_norm.zero_grad();
    feed_forward.zero_grad();
    feed_forward_norm.zero_grad();
  }

  void apply_gradients(float learning_rate) {
    attention.apply_gradients(learning_rate);

    attention_norm.apply_gradients(learning_rate);

    feed_forward.apply_gradients(learning_rate);

    feed_forward_norm.apply_gradients(learning_rate);
  }

  int parameter_count() const {
    return attention.parameter_count() + attention_norm.parameter_count() +
           feed_forward.parameter_count() + feed_forward_norm.parameter_count();
  }

  void export_weights(Vector<float> &out) const {
    attention.export_weights(out);
    attention_norm.export_weights(out);
    feed_forward.export_weights(out);
    feed_forward_norm.export_weights(out);
  }

  bool import_weights(const Vector<float> &in, int &cursor) {
    if (!attention.import_weights(in, cursor))
      return false;

    if (!attention_norm.import_weights(in, cursor))
      return false;

    if (!feed_forward.import_weights(in, cursor))
      return false;

    if (!feed_forward_norm.import_weights(in, cursor))
      return false;

    return true;
  }

  void update_weights(const Vector<float> &query_gradients,
                      const Vector<float> &key_gradients,
                      const Vector<float> &value_gradients,
                      const Vector<float> &attention_gamma_gradients,
                      const Vector<float> &attention_beta_gradients,
                      const Vector<float> &first_weight_gradients,
                      const Vector<float> &first_bias_gradients,
                      const Vector<float> &second_weight_gradients,
                      const Vector<float> &second_bias_gradients,
                      const Vector<float> &feed_forward_gamma_gradients,
                      const Vector<float> &feed_forward_beta_gradients,
                      float learning_rate) {

    attention.update_weights(query_gradients, key_gradients, value_gradients,
                             learning_rate);

    attention_norm.update_weights(attention_gamma_gradients,
                                  attention_beta_gradients, learning_rate);

    feed_forward.update_weights(first_weight_gradients, first_bias_gradients,
                                second_weight_gradients, second_bias_gradients,
                                learning_rate);

    feed_forward_norm.update_weights(feed_forward_gamma_gradients,
                                     feed_forward_beta_gradients,
                                     learning_rate);
  }
};
