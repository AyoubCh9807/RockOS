#include "embedding.hpp"
#include "entity_classifier.hpp"
#include "intent_classifier.hpp"
#include "model_io.hpp"
#include "positional_encoder.hpp"
#include "tokenizer.hpp"
#include "training_dataset.hpp"
#include "transform_layer.hpp"
#include "transformer_input.hpp"
#include "vocabulary.hpp"

#include <iostream>

struct Prediction {
  IntentClassifier::Intent intent;
  IntentClassifier::Entity entity;
};

static Prediction predict(
    const char *text,
    Tokenizer &tokenizer,
    Embedding &embedding,
    PositionalEncoder &pos_encoder,
    Vector<TransformerLayer *> &layers,
    IntentClassifier &classifier,
    EntityClassifier &entity_classifier) {

  String phrase(text);

  Vector<Token> &tokens =
      tokenizer.tokenize(phrase);

  if (tokens.size() == 0)
    return {IntentClassifier::Intent::UNKNOWN, IntentClassifier::Entity::NONE};

  TransformerInput input(
      embedding,
      pos_encoder);

  input.build(tokens);

  Vector<Vector<float>> sequence =
      input.get();

  for (int i = 0; i < layers.size(); i++)
    sequence =
        layers[i]->forward(sequence);

  // Both heads predict off the same final sequence, independently.
  IntentClassifier::Intent intent = classifier.predict(sequence);
  IntentClassifier::Entity entity = entity_classifier.predict(sequence);

  return {intent, entity};
}

static const char *intent_name(
    IntentClassifier::Intent intent) {

  switch (intent) {

  case IntentClassifier::Intent::GREETING:
    return "GREETING";

  case IntentClassifier::Intent::MEMORY_USAGE:
    return "MEMORY_USAGE";

  case IntentClassifier::Intent::TIME:
    return "TIME";

  case IntentClassifier::Intent::UPTIME:
    return "UPTIME";

  case IntentClassifier::Intent::OPEN_APP:
    return "OPEN_APP";

  case IntentClassifier::Intent::CLOSE_WINDOW:
    return "CLOSE_WINDOW";

  case IntentClassifier::Intent::MINIMIZE_WINDOW:
    return "MINIMIZE_WINDOW";

  case IntentClassifier::Intent::HELP:
    return "HELP";

  default:
    return "UNKNOWN";
  }
}

static const char *entity_name(
    IntentClassifier::Entity entity) {

  switch (entity) {

  case IntentClassifier::Entity::CALCULATOR:
    return "CALCULATOR";

  case IntentClassifier::Entity::MATRIX:
    return "MATRIX";

  case IntentClassifier::Entity::TERMINAL:
    return "TERMINAL";

  case IntentClassifier::Entity::BROWSER:
    return "BROWSER";

  case IntentClassifier::Entity::ROCK_AI:
    return "ROCK_AI";

  case IntentClassifier::Entity::TYRANT:
    return "TYRANT";

  case IntentClassifier::Entity::SETTINGS:
    return "SETTINGS";

  case IntentClassifier::Entity::MUSIC_PLAYER:
    return "MUSIC_PLAYER";

  default:
    return "NONE";
  }
}

static void respond(
    IntentClassifier::Intent intent,
    IntentClassifier::Entity entity) {

  switch (intent) {

  case IntentClassifier::Intent::GREETING:
    std::cout
        << "Rock AI: Yo! What's up? 🤘\n";
    break;

  case IntentClassifier::Intent::MEMORY_USAGE:
    std::cout
        << "Rock AI: You want to check memory usage.\n";
    break;

  case IntentClassifier::Intent::TIME:
    std::cout
        << "Rock AI: You want to know the time.\n";
    break;

  case IntentClassifier::Intent::UPTIME:
    std::cout
        << "Rock AI: You want to know how long Rock OS has been running.\n";
    break;

  case IntentClassifier::Intent::OPEN_APP:
    std::cout
        << "Rock AI: You want to open " << entity_name(entity) << ".\n";
    break;

  case IntentClassifier::Intent::CLOSE_WINDOW:
    std::cout
        << "Rock AI: You want to close " << entity_name(entity) << ".\n";
    break;

  case IntentClassifier::Intent::MINIMIZE_WINDOW:
    std::cout
        << "Rock AI: You want to minimize " << entity_name(entity) << ".\n";
    break;

  case IntentClassifier::Intent::HELP:
    std::cout
        << "Rock AI: You want some help.\n";
    break;

  default:
    std::cout
        << "Rock AI: I don't understand that yet.\n";
    break;
  }
}

int main() {

  std::cout
      << "Loading Rock AI evaluation environment...\n";

  Vocabulary vocab;

  Tokenizer tokenizer(vocab);

  TrainingDataset dataset(
      tokenizer,
      vocab);

  dataset.register_examples();

  std::cout
      << "Registered vocabulary: "
      << vocab.size()
      << " words.\n";

  Embedding embedding(
      vocab.size());

  PositionalEncoder pos_encoder;

  TransformerLayer layer;

  Vector<TransformerLayer *> layers;

  layers.push_back(&layer);

  IntentClassifier classifier;
  EntityClassifier entity_classifier;

  constexpr const char *MODEL_FILE =
      "rock_ai.model";

  if (!ModelIO::load(
          MODEL_FILE,
          embedding,
          layers,
          classifier,
          entity_classifier)) {

    std::cout
        << "ERROR: Could not load Rock AI model.\n";

    std::cout
        << "Train the model first with main.cpp.\n";

    return 1;
  }

  std::cout
      << "Rock AI model loaded successfully.\n";

  std::cout
      << "\nRock AI evaluation mode.\n";

  std::cout
      << "Type 'quit' to exit.\n\n";

  char input[256];

  while (true) {

    std::cout << "You: ";

    std::cin.getline(
        input,
        sizeof(input));

    if (input[0] == '\0')
      continue;

    if (input[0] == 'q' &&
        input[1] == 'u' &&
        input[2] == 'i' &&
        input[3] == 't' &&
        input[4] == '\0') {

      break;
    }

    Prediction prediction =
        predict(
            input,
            tokenizer,
            embedding,
            pos_encoder,
            layers,
            classifier,
            entity_classifier);

    std::cout
        << "Intent: "
        << intent_name(prediction.intent)
        << " ("
        << static_cast<int>(prediction.intent)
        << ")  Entity: "
        << entity_name(prediction.entity)
        << " ("
        << static_cast<int>(prediction.entity)
        << ")\n";

    respond(prediction.intent, prediction.entity);

    std::cout << "\n";
  }

  std::cout
      << "\nRock AI evaluation finished. 🤘\n";

  return 0;
}
