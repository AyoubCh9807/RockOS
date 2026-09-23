#include "embedding.hpp"
#include "entity_classifier.hpp"
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

  TrainingDataset dataset(tokenizer, vocab);

  dataset.register_examples();

  Embedding embedding(vocab.size());

  PositionalEncoder pos_encoder;

  TransformerLayer layer;

  Vector<TransformerLayer *> layers;

  layers.push_back(&layer);

  IntentClassifier classifier;
  EntityClassifier entity_classifier;

  constexpr const char *MODEL_FILE = "rock_ai.model";

  if (ModelIO::load(MODEL_FILE, embedding, layers, classifier,
                    entity_classifier)) {

    std::cout << "Loaded existing Rock AI model.\n";

  } else {

    std::cout << "No compatible Rock AI model found.\n";
    std::cout << "Starting with fresh weights.\n";
  }

  Trainer trainer(
      tokenizer,
      embedding,
      pos_encoder,
      layers,
      classifier,
      entity_classifier);

  constexpr int epochs = 150;
  constexpr float learning_rate = 0.0008f;

  std::cout
      << "Training examples: "
      << dataset.size()
      << "\n";

  std::cout
      << "Starting training for "
      << epochs
      << " epochs...\n";

  float best_loss =
      std::numeric_limits<float>::infinity();

  int best_epoch = -1;

  float final_loss = trainer.train(
      dataset,
      epochs,
      learning_rate,
      [&](int epoch, float loss) {

        if (std::isfinite(loss) &&
            loss < best_loss) {

          best_loss = loss;
          best_epoch = epoch + 1;

          std::cout
              << "  -> New best loss ("
              << loss
              << ") at epoch "
              << best_epoch
              << " - saving checkpoint.\n";

          if (!ModelIO::save(
                  MODEL_FILE,
                  embedding,
                  layers,
                  classifier,
                  entity_classifier)) {

            std::cout
                << "  -> WARNING: failed to save checkpoint.\n";
          }
        }
      },
      0.995f);

  std::cout
      << "\nTraining complete!\n";

  std::cout
      << "Final average loss: "
      << final_loss
      << "\n";

  if (best_epoch == -1) {

    std::cout
        << "No stable epoch was ever reached - "
           "nothing was saved.\n";

    return 1;
  }

  std::cout
      << "Best checkpoint was epoch "
      << best_epoch
      << " with loss "
      << best_loss
      << " - that's what's saved in "
      << MODEL_FILE
      << ".\n";

  if (!std::isfinite(final_loss) ||
      final_loss > best_loss * 2.0f) {

    std::cout
        << "(Training moved past its best point before "
           "finishing - that's fine, the saved checkpoint "
           "is still the good one.)\n";
  }

  std::cout
      << "\nTraining session complete. 🤘\n";

  return 0;
}
