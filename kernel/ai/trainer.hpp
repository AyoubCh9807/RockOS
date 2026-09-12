#pragma once

#include "../containers/vector.hpp"
#include "../utils/math_utils.hpp"

#include "embedding.hpp"
#include "intent_classifier.hpp"
#include "positional_encoder.hpp"
#include "token.hpp"
#include "tokenizer.hpp"
#include "training_dataset.hpp"
#include "transform_layer.hpp"
#include "transformer_input.hpp"

#include <iostream>

class Trainer {
private:
  static constexpr float GRADIENT_CLIP = 1.0f;

  Tokenizer &tokenizer;
  Embedding &embedding;
  TransformerInput transformer_input;
  Vector<TransformerLayer *> layers;
  IntentClassifier &classifier;

  float natural_log(float x) {
    if (x < 0.0001f)
      x = 0.0001f;

    float y = 0.0f;

    for (int i = 0; i < 30; i++) {
      float e = MathUtils::exp(y);
      y = y - (e - x) / e;
    }

    return y;
  }

public:
  Trainer(Tokenizer &tokenizer, Embedding &embedding,
          PositionalEncoder &positional_encoder,
          Vector<TransformerLayer *> layers, IntentClassifier &classifier)
      : tokenizer(tokenizer),
        embedding(embedding),
        transformer_input(embedding, positional_encoder),
        layers(layers),
        classifier(classifier) {}

  float train_on_example(TrainingExample<IntentClassifier::Intent> &example,
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

    float loss_value = -natural_log(probs[label_index]);

    Vector<float> d_logits =
        classifier.gradient_from_label(logits, example.label);

    embedding.zero_grad();
    classifier.zero_grad();

    for (int l = 0; l < layers.size(); l++)
      layers[l]->zero_grad();

    Vector<Vector<float>> d_sequence =
        classifier.backward(d_logits);

    for (int l = layers.size() - 1; l >= 0; l--)
      d_sequence = layers[l]->backward(d_sequence);

    transformer_input.backward(d_sequence, tokens);

    embedding.apply_gradients(learning_rate);
    classifier.apply_gradients(learning_rate);

    for (int l = 0; l < layers.size(); l++)
      layers[l]->apply_gradients(learning_rate);

    return loss_value;
  }

  float train(TrainingDataset<IntentClassifier::Intent> &dataset, int epochs,
              float learning_rate) {
    if (dataset.size() == 0 || epochs <= 0)
      return 0.0f;

    float last_epoch_average_loss = 0.0f;

    for (int epoch = 0; epoch < epochs; epoch++) {
      float total_loss = 0.0f;

      for (int i = 0; i < dataset.size(); i++) {
        float loss = train_on_example(dataset.get(i), learning_rate);

        if (loss != loss) {
          std::cout << "NaN detected at epoch "
                    << (epoch + 1)
                    << ", example "
                    << i
                    << "\n";

          return last_epoch_average_loss;
        }

        total_loss += loss;

        if (i % 25 == 0) {
          std::cout << "Epoch "
                    << (epoch + 1)
                    << ", example "
                    << i
                    << " loss: "
                    << loss
                    << "\n";
        }
      }

      last_epoch_average_loss =
          total_loss / dataset.size();

      std::cout << "Epoch "
                << (epoch + 1)
                << " loss: "
                << last_epoch_average_loss
                << "\n";
    }

    return last_epoch_average_loss;
  }
};
