#pragma once

#include "../containers/vector.hpp"
#include "../random/random.hpp"

#include "embedding.hpp"
#include "entity_classifier.hpp"
#include "intent_classifier.hpp"
#include "positional_encoder.hpp"
#include "token.hpp"
#include "tokenizer.hpp"
#include "training_dataset.hpp"
#include "transform_layer.hpp"
#include "transformer_input.hpp"

#include <cmath>
#include <functional>
#include <iostream>
#include <limits>

class Trainer {
private:
  static constexpr float GRADIENT_CLIP = 1.0f;
  static constexpr float GLOBAL_GRADIENT_CLIP = 4.0f;

  static constexpr int MAX_OVERSAMPLE_REPEATS = 6;

  Tokenizer &tokenizer;
  Embedding &embedding;
  TransformerInput transformer_input;
  Vector<TransformerLayer *> layers;
  IntentClassifier &intent_classifier;
  EntityClassifier &entity_classifier;

  static void shuffle_indices(Vector<int> &indices) {
    for (int i = indices.size() - 1; i > 0; i--) {
      int j = static_cast<int>(Random::next() % static_cast<u32>(i + 1));

      int temp = indices[i];
      indices[i] = indices[j];
      indices[j] = temp;
    }
  }

  static Vector<int> build_oversampled_indices(TrainingDataset &dataset) {
    constexpr int ENTITY_COUNT =
        static_cast<int>(IntentClassifier::Entity::COUNT);

    int counts[ENTITY_COUNT];
    for (int c = 0; c < ENTITY_COUNT; c++)
      counts[c] = 0;

    for (int i = 0; i < dataset.size(); i++)
      counts[static_cast<int>(dataset.get(i).entity)]++;

    int max_count = 0;
    for (int c = 0; c < ENTITY_COUNT; c++)
      if (counts[c] > max_count)
        max_count = counts[c];

    Vector<int> weighted_indices;

    for (int i = 0; i < dataset.size(); i++) {
      int c = static_cast<int>(dataset.get(i).entity);

      int repeats = counts[c] > 0 ? max_count / counts[c] : 1;

      if (repeats < 1)
        repeats = 1;
      if (repeats > MAX_OVERSAMPLE_REPEATS)
        repeats = MAX_OVERSAMPLE_REPEATS;

      for (int r = 0; r < repeats; r++)
        weighted_indices.push_back(i);
    }

    return weighted_indices;
  }

  static void clip_gradient(Vector<float> &grad, float max_norm) {
    float norm_sq = 0.0f;

    for (int i = 0; i < grad.size(); i++)
      norm_sq += grad[i] * grad[i];

    float norm = std::sqrt(norm_sq);

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

  static void clip_global_norm(Vector<Vector<float> *> &grads, float max_norm) {
    float norm_sq = 0.0f;

    for (int v = 0; v < grads.size(); v++) {
      Vector<float> &g = *grads[v];

      for (int i = 0; i < g.size(); i++)
        norm_sq += g[i] * g[i];
    }

    float norm = std::sqrt(norm_sq);

    if (!std::isfinite(norm)) {
      for (int v = 0; v < grads.size(); v++) {
        Vector<float> &g = *grads[v];

        for (int i = 0; i < g.size(); i++)
          g[i] = 0.0f;
      }

      return;
    }

    if (norm > max_norm && norm > 0.0f) {
      float scale = max_norm / norm;

      for (int v = 0; v < grads.size(); v++) {
        Vector<float> &g = *grads[v];

        for (int i = 0; i < g.size(); i++)
          g[i] *= scale;
      }
    }
  }

public:
  struct TrainStepResult {
    float total_loss;
    float intent_loss;
    float entity_loss;
  };

  Trainer(Tokenizer &tokenizer, Embedding &embedding,
          PositionalEncoder &positional_encoder,
          Vector<TransformerLayer *> layers, IntentClassifier &intent_classifier,
          EntityClassifier &entity_classifier)
      : tokenizer(tokenizer), embedding(embedding),
        transformer_input(embedding, positional_encoder), layers(layers),
        intent_classifier(intent_classifier),
        entity_classifier(entity_classifier) {}

  TrainStepResult train_on_example(
      TrainingExample<IntentClassifier::Intent, IntentClassifier::Entity>
          &example,
      float learning_rate) {

    Vector<Token> &tokens = tokenizer.tokenize(example.text);

    if (tokens.size() == 0)
      return {0.0f, 0.0f, 0.0f};

    transformer_input.build(tokens);

    Vector<Vector<float>> sequence = transformer_input.get();

    for (int l = 0; l < layers.size(); l++)
      sequence = layers[l]->forward(sequence);

    Vector<float> intent_logits = intent_classifier.classify(sequence);
    Vector<float> intent_probs = intent_classifier.softmax(intent_logits);

    int intent_label = static_cast<int>(example.intent);
    float intent_probability = intent_probs[intent_label];
    if (intent_probability < 0.0001f)
      intent_probability = 0.0001f;
    float intent_loss = -std::log(intent_probability);

    Vector<float> entity_logits = entity_classifier.classify(sequence);
    Vector<float> entity_probs = entity_classifier.softmax(entity_logits);

    int entity_label = static_cast<int>(example.entity);
    float entity_probability = entity_probs[entity_label];
    if (entity_probability < 0.0001f)
      entity_probability = 0.0001f;
    float entity_loss = -std::log(entity_probability);

    Vector<float> d_intent_logits =
        intent_classifier.gradient_from_label(intent_logits, example.intent);
    clip_gradient(d_intent_logits, GRADIENT_CLIP);

    Vector<float> d_entity_logits =
        entity_classifier.gradient_from_label(entity_logits, example.entity);
    clip_gradient(d_entity_logits, GRADIENT_CLIP);

    embedding.zero_grad();
    intent_classifier.zero_grad();
    entity_classifier.zero_grad();

    for (int l = 0; l < layers.size(); l++)
      layers[l]->zero_grad();

    Vector<Vector<float>> d_sequence_from_intent =
        intent_classifier.backward(d_intent_logits);

    Vector<Vector<float>> d_sequence_from_entity =
        entity_classifier.backward(d_entity_logits);

    Vector<Vector<float>> d_sequence;

    for (int t = 0; t < d_sequence_from_intent.size(); t++) {
      Vector<float> combined;
      combined.resize(d_sequence_from_intent[t].size(), 0.0f);

      for (int k = 0; k < combined.size(); k++)
        combined[k] = d_sequence_from_intent[t][k] + d_sequence_from_entity[t][k];

      d_sequence.push_back(combined);
    }

    for (int l = layers.size() - 1; l >= 0; l--)
      d_sequence = layers[l]->backward(d_sequence);

    transformer_input.backward(d_sequence, tokens);

    Vector<Vector<float> *> all_gradients;

    embedding.collect_gradients(all_gradients);
    intent_classifier.collect_gradients(all_gradients);
    entity_classifier.collect_gradients(all_gradients);

    for (int l = 0; l < layers.size(); l++)
      layers[l]->collect_gradients(all_gradients);

    clip_global_norm(all_gradients, GLOBAL_GRADIENT_CLIP);

    embedding.apply_gradients(learning_rate);
    intent_classifier.apply_gradients(learning_rate);
    entity_classifier.apply_gradients(learning_rate);

    for (int l = 0; l < layers.size(); l++)
      layers[l]->apply_gradients(learning_rate);

    return {intent_loss + entity_loss, intent_loss, entity_loss};
  }

  // early_stop_patience: if > 0, training stops once
  // early_stop_patience consecutive epochs pass without the epoch-average
  // loss improving on the best one seen so far. 0 (default) disables
  // this and preserves the old always-run-every-epoch behavior.
  //
  // This is what your last run needed: epoch 110 hit the best loss
  // (0.866244), and the following 40 epochs never beat it - they just
  // kept applying full-magnitude gradient steps (GRADIENT_CLIP/
  // GLOBAL_GRADIENT_CLIP bound each STEP's size, they don't bound how
  // far a long, aimless walk of thousands of such steps can drift) until
  // the network wandered into the degenerate all-logits-saturated state
  // you saw at epoch 150. main.cpp already only keeps the best
  // checkpoint, so that collapse never reached rock_ai.model - but there
  // was no reason to spend 40 epochs' worth of compute walking toward it
  // either.
  float train(TrainingDataset &dataset, int epochs, float learning_rate,
              std::function<void(int, float)> on_epoch_end = nullptr,
              float learning_rate_decay = 1.0f,
              int early_stop_patience = 0) {

    if (dataset.size() == 0 || epochs <= 0)
      return 0.0f;

    float last_epoch_average_loss = 0.0f;

    Vector<int> indices = build_oversampled_indices(dataset);

    std::cout << "Entity-balanced epoch size: " << indices.size()
              << " steps (raw dataset size: " << dataset.size() << ")\n";

    float best_seen_loss = std::numeric_limits<float>::infinity();
    int epochs_without_improvement = 0;

    for (int epoch = 0; epoch < epochs; epoch++) {
      shuffle_indices(indices);

      float total_loss = 0.0f;

      for (int step = 0; step < indices.size(); step++) {
        int i = indices[step];

        TrainStepResult result = train_on_example(dataset.get(i), learning_rate);

        if (!std::isfinite(result.total_loss)) {
          std::cout << "NaN/Inf detected at epoch " << (epoch + 1)
                    << ", example " << i << "\n";

          return last_epoch_average_loss;
        }

        total_loss += result.total_loss;

        if (step % 25 == 0) {
          std::cout << "Epoch " << (epoch + 1) << ", example " << step
                    << " loss: " << result.total_loss
                    << " (intent: " << result.intent_loss
                    << ", entity: " << result.entity_loss << ")\n";
        }
      }

      last_epoch_average_loss = total_loss / static_cast<float>(indices.size());

      std::cout << "Epoch " << (epoch + 1)
                << " loss: " << last_epoch_average_loss << "\n";

      if (on_epoch_end)
        on_epoch_end(epoch, last_epoch_average_loss);

      if (early_stop_patience > 0) {
        if (last_epoch_average_loss < best_seen_loss) {
          best_seen_loss = last_epoch_average_loss;
          epochs_without_improvement = 0;
        } else {
          epochs_without_improvement++;

          if (epochs_without_improvement >= early_stop_patience) {
            std::cout << "No improvement for " << early_stop_patience
                      << " epochs (best average loss " << best_seen_loss
                      << ") - stopping early at epoch " << (epoch + 1)
                      << ".\n";
            break;
          }
        }
      }

      learning_rate *= learning_rate_decay;
    }

    return last_epoch_average_loss;
  }
};
