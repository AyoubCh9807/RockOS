#pragma once

#include "../containers/vector.hpp"
#include "embedding.hpp"
#include "positional_encoder.hpp"
#include "token.hpp"

class TransformerInput {
private:
  static constexpr int EMBEDDING_DIMENSION = 128;

  Embedding &embedding;
  PositionalEncoder &positional_encoder;

  // NOTE: originally this was a single flat Vector<float> holding every
  // token's numbers back to back. That doesn't match what TransformerLayer
  // expects currently (a Vector<Vector<float>>, one 128-number vector per
  // token), so it's been changed to build one sub-vector per token instead.
  Vector<Vector<float>> input;

public:
  TransformerInput(Embedding &embedding, PositionalEncoder &positional_encoder)
      : embedding(embedding), positional_encoder(positional_encoder) {}

  void build(Vector<Token> &tokens) {
    input.clear();

    for (int i = 0; i < tokens.size(); i++) {
      int token_id = tokens[i].id;

      if (token_id < 0 || token_id >= embedding.size())
        debug_fail();

      float *emb = embedding.get_embedding(token_id);

      if (!emb)
        debug_fail();

      Vector<float> positional_encoding = positional_encoder.encode(i);

      if (positional_encoding.size() != EMBEDDING_DIMENSION)
        debug_fail();

      Vector<float> token_vector;
      token_vector.reserve(EMBEDDING_DIMENSION);

      for (int j = 0; j < EMBEDDING_DIMENSION; j++)
        token_vector.push_back(emb[j] + positional_encoding[j]);

      input.push_back(token_vector);
    }
  }

  Vector<Vector<float>> &get() { return input; }

  int size() const { return input.size(); }

  // Backpropagation 

  // d_input holds one gradient vector per token (same shape as what
  // build() produces). Positional encodings have no learnable weights, so
  // their share of the gradient is simply not used for anything, only the
  // embedding table gets updated.
  void backward(const Vector<Vector<float>> &d_input, Vector<Token> &tokens) {
    for (int i = 0; i < tokens.size(); i++)
      embedding.backward(tokens[i].id, d_input[i]);
  }

  static void debug_fail() {
    volatile int *crash = nullptr;
    *crash = 1;
  }
};
