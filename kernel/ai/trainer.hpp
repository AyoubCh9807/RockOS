#pragma once

#include "../containers/vector.hpp"
#include "../random/random.hpp"

#include "embedding.hpp"
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

class Trainer {
private:
  // Clip applied to the classifier's OWN output gradient (d_logits) as
  // it enters the backward pass. Kept as an extra early backstop, but
  // it is no longer the main defense - see GLOBAL_GRADIENT_CLIP below.
  static constexpr float GRADIENT_CLIP = 1.0f;

  // Clip applied to the L2 norm of EVERY gradient in the network
  // combined - embedding, every transformer layer, and the classifier -
  // treated as one single vector. This replaces the earlier approach of
  // clipping each module's gradient vector independently: per-vector
  // clipping bounds each vector in isolation, but a family of vectors
  // that are each individually "fine" can still combine to push the
  // network too far in one step, and any single vector someone forgets
  // to wire up a clip for becomes a silent hole (which is exactly what
  // kept happening - IntentClassifier's own weights, then LayerNorm's
  // gamma, then something else at epoch 70). Computing and clipping one
  // combined norm across everything closes off that whole class of bug
  // at a single point instead of module-by-module.
  //
  // NOTE ON THE VALUE: this norm is computed across tens of thousands
  // of parameters combined (every weight matrix in every layer), not a
  // single small vector, so it needs to be much larger than the 1.0
  // that made sense for one attention weight matrix in isolation.
  // Too small here doesn't cause instability - it just crushes every
  // gradient step down to near-zero magnitude, which looks like loss
  // plateauing early and never improving rather than diverging.
  // Too large stops actually protecting against the blow-up this exists
  // to prevent - 10.0 still let the model diverge, just later (epoch 80
  // instead of epoch 4). This is why learning rate decay (see train()
  // below) matters as a second, complementary lever: a single fixed
  // clip has to be safe for the LARGEST gradients seen across all 150
  // epochs, but late-training instability is exactly when a shrinking
  // learning rate helps most, so the two together are more robust than
  // pushing either one to an extreme on its own.
  static constexpr float GLOBAL_GRADIENT_CLIP = 4.0f;

  Tokenizer &tokenizer;
  Embedding &embedding;
  TransformerInput transformer_input;
  Vector<TransformerLayer *> layers;
  IntentClassifier &classifier;

  // Fisher-Yates shuffle of an index array, so each epoch walks the
  // dataset in a different order instead of the fixed class-by-class
  // order it was registered in.
  static void shuffle_indices(Vector<int> &indices) {
    for (int i = indices.size() - 1; i > 0; i--) {
      int j = static_cast<int>(Random::next() % static_cast<u32>(i + 1));

      int temp = indices[i];
      indices[i] = indices[j];
      indices[j] = temp;
    }
  }

  // Clips a gradient vector in place so its L2 norm never exceeds
  // max_norm. Still used for the d_logits entry-point clip below.
  static void clip_gradient(Vector<float> &grad, float max_norm) {
    float norm_sq = 0.0f;

    for (int i = 0; i < grad.size(); i++)
      norm_sq += grad[i] * grad[i];

    float norm = std::sqrt(norm_sq);

    if (norm > max_norm && norm > 0.0f) {
      float scale = max_norm / norm;

      for (int i = 0; i < grad.size(); i++)
        grad[i] *= scale;
    }
  }

  // Computes the L2 norm of every gradient vector in grads combined (as
  // if they were all one flat vector), and if it exceeds max_norm,
  // scales every value in every vector down by the same factor so the
  // combined norm becomes exactly max_norm. This must run after every
  // module's backward() has finished accumulating its gradients, and
  // before any module's apply_gradients() is called.
  static void clip_global_norm(Vector<Vector<float> *> &grads,
                               float max_norm) {
    float norm_sq = 0.0f;

    for (int v = 0; v < grads.size(); v++) {
      Vector<float> &g = *grads[v];

      for (int i = 0; i < g.size(); i++)
        norm_sq += g[i] * g[i];
    }

    float norm = std::sqrt(norm_sq);

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
  Trainer(Tokenizer &tokenizer,
          Embedding &embedding,
          PositionalEncoder &positional_encoder,
          Vector<TransformerLayer *> layers,
          IntentClassifier &classifier)
      : tokenizer(tokenizer),
        embedding(embedding),
        transformer_input(embedding, positional_encoder),
        layers(layers),
        classifier(classifier) {}

  float train_on_example(
      TrainingExample<IntentClassifier::Intent> &example,
      float learning_rate) {

    Vector<Token> &tokens = tokenizer.tokenize(example.text);

    if (tokens.size() == 0)
      return 0.0f;

    transformer_input.build(tokens);

    Vector<Vector<float>> sequence = transformer_input.get();

    for (int l = 0; l < layers.size(); l++)
      sequence = layers[l]->forward(sequence);

    Vector<float> logits = classifier.classify(sequence);

    Vector<float> probs = classifier.softmax(logits);

    int label_index = static_cast<int>(example.label);

    float probability = probs[label_index];

    if (probability < 0.0001f)
      probability = 0.0001f;

    float loss_value = -std::log(probability);

    Vector<float> d_logits =
        classifier.gradient_from_label(logits, example.label);

    // Clip right at the source of the gradient signal - everything
    // downstream (classifier, layers, embedding) scales off of this,
    // so bounding it here keeps the whole backward pass stable. This is
    // an early backstop; the global clip below is the main defense.
    clip_gradient(d_logits, GRADIENT_CLIP);

    embedding.zero_grad();
    classifier.zero_grad();

    for (int l = 0; l < layers.size(); l++)
      layers[l]->zero_grad();

    Vector<Vector<float>> d_sequence =
        classifier.backward(d_logits);

    for (int l = layers.size() - 1; l >= 0; l--)
      d_sequence = layers[l]->backward(d_sequence);

    transformer_input.backward(d_sequence, tokens);

    // Gather every gradient vector in the whole network - embedding,
    // every transformer layer's attention/norm/feed-forward, and the
    // classifier - and clip their COMBINED L2 norm as one unit. This
    // runs after all backward() calls above (so every gradient has
    // finished accumulating) and before any apply_gradients() call
    // below (so nothing has been applied yet).
    Vector<Vector<float> *> all_gradients;

    embedding.collect_gradients(all_gradients);
    classifier.collect_gradients(all_gradients);

    for (int l = 0; l < layers.size(); l++)
      layers[l]->collect_gradients(all_gradients);

    clip_global_norm(all_gradients, GLOBAL_GRADIENT_CLIP);

    embedding.apply_gradients(learning_rate);
    classifier.apply_gradients(learning_rate);

    for (int l = 0; l < layers.size(); l++)
      layers[l]->apply_gradients(learning_rate);

    return loss_value;
  }

  float train(TrainingDataset<IntentClassifier::Intent> &dataset,
              int epochs,
              float learning_rate,
              // Called after every epoch with (epoch_index, epoch_loss).
              // Use this to save a checkpoint whenever loss improves, so
              // a later collapse doesn't erase a good earlier state.
              std::function<void(int, float)> on_epoch_end = nullptr,
              // Multiplies learning_rate by this factor at the end of
              // every epoch. Defaults to 1.0 (no decay), matching the
              // old behavior. A value like 0.98 means by epoch 150,
              // learning_rate has shrunk to roughly its starting value
              // times 0.98^150 (~5%). This complements GLOBAL_GRADIENT_CLIP:
              // the clip bounds how big one step's gradient direction can
              // be, but the actual size of the weight update is
              // learning_rate * (clipped gradient) - shrinking
              // learning_rate over time makes every later step smaller
              // and safer without having to make the clip itself so
              // tight that it stalls learning early on, when larger
              // steps are still useful.
              float learning_rate_decay = 1.0f) {

    if (dataset.size() == 0 || epochs <= 0)
      return 0.0f;

    float last_epoch_average_loss = 0.0f;

    Vector<int> indices;
    for (int i = 0; i < dataset.size(); i++)
      indices.push_back(i);

    for (int epoch = 0; epoch < epochs; epoch++) {

      shuffle_indices(indices);

      float total_loss = 0.0f;

      for (int step = 0; step < dataset.size(); step++) {
        int i = indices[step];

        float loss =
            train_on_example(dataset.get(i), learning_rate);

        if (!std::isfinite(loss)) {
          std::cout
              << "NaN/Inf detected at epoch "
              << (epoch + 1)
              << ", example "
              << i
              << "\n";

          return last_epoch_average_loss;
        }

        total_loss += loss;

        if (step % 25 == 0) {
          std::cout
              << "Epoch "
              << (epoch + 1)
              << ", example "
              << step
              << " loss: "
              << loss
              << "\n";
        }
      }

      last_epoch_average_loss =
          total_loss / static_cast<float>(dataset.size());

      std::cout
          << "Epoch "
          << (epoch + 1)
          << " loss: "
          << last_epoch_average_loss
          << "\n";

      if (on_epoch_end)
        on_epoch_end(epoch, last_epoch_average_loss);

      learning_rate *= learning_rate_decay;
    }

    return last_epoch_average_loss;
  }
};
