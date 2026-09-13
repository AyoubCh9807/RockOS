#include "embedding.hpp"
#include "intent_classifier.hpp"
#include "model_io.hpp"
#include "positional_encoder.hpp"
#include "tokenizer.hpp"
#include "trainer.hpp"
#include "training_dataset.hpp"
#include "transform_layer.hpp"
#include "vocabulary.hpp"

#include <cmath>
#include <iostream>
#include <limits>

int main() {

  std::cout << "Initializing Rock AI Training Pipeline...\n";

  Vocabulary vocab;
  Tokenizer tokenizer(vocab);

  TrainingDataset<IntentClassifier::Intent> dataset(tokenizer, vocab);

  dataset.register_examples();

  Embedding embedding(vocab.size());

  PositionalEncoder pos_encoder;

  TransformerLayer layer;

  Vector<TransformerLayer *> layers;

  layers.push_back(&layer);

  IntentClassifier classifier;

  constexpr const char *MODEL_FILE = "rock_ai.model";

  if (ModelIO::load(MODEL_FILE, embedding, layers, classifier)) {

    std::cout << "Loaded existing Rock AI model.\n";

  } else {

    std::cout << "No compatible Rock AI model found.\n";

    std::cout << "Starting with fresh weights.\n";
  }

  Trainer trainer(tokenizer, embedding, pos_encoder, layers, classifier);

  constexpr int epochs = 150;
  constexpr float learning_rate = 0.0008f;

  std::cout << "Training examples: " << dataset.size() << "\n";

  std::cout << "Starting training for " << epochs << " epochs...\n";

  float best_loss = std::numeric_limits<float>::infinity();
  int best_epoch = -1;

  float final_loss = trainer.train(
      dataset, epochs, learning_rate,
      [&](int epoch, float loss) {
        // Save the moment we see a new best - this way, if training
        // later collapses or diverges, we've already captured the good
        // state on disk instead of losing it.
        if (std::isfinite(loss) && loss < best_loss) {
          best_loss = loss;
          best_epoch = epoch + 1;

          std::cout
              << "  -> New best loss (" << loss
              << ") at epoch " << best_epoch
              << " - saving checkpoint.\n";

          if (!ModelIO::save(MODEL_FILE, embedding, layers, classifier)) {
            std::cout << "  -> WARNING: failed to save checkpoint.\n";
          }
        }
      },
      // Shrinks learning_rate by 0.5% every epoch. By epoch 150 it's
      // roughly 0.995^150 (~47%) of its starting value - gentle enough
      // that the model can keep making real progress through most of
      // training, but still tapers off late steps for stability.
      // (0.98 was tried first and decayed too hard - by epoch 107 the
      // learning rate had already shrunk to ~11% of its start, which is
      // why loss plateaued around 1.77-1.8 for the whole second half of
      // that run instead of continuing to drop.)
      0.995f);

  std::cout << "\nTraining complete!\n";

  std::cout << "Final average loss: " << final_loss << "\n";

  if (best_epoch == -1) {

    std::cout << "No stable epoch was ever reached - nothing was saved.\n";

    return 1;
  }

  std::cout
      << "Best checkpoint was epoch " << best_epoch
      << " with loss " << best_loss
      << " - that is what's saved in " << MODEL_FILE << ".\n";

  if (!std::isfinite(final_loss) || final_loss > best_loss * 2.0f) {
    std::cout
        << "(Training moved past its best point before finishing - "
           "that's fine, the saved checkpoint is still the good one.)\n";
  }

  std::cout << "\nTraining session complete. 🤘\n";

  return 0;
}
