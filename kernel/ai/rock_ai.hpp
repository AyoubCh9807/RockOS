#pragma once

#include "../containers/vector.hpp"
#include "rock_ai_weights.hpp"

#include "embedding.hpp"
#include "entity_classifier.hpp"
#include "intent_classifier.hpp"
// #include "model_io.hpp"
#include "positional_encoder.hpp"
#include "rock_ai_model.hpp"
#include "token.hpp"
#include "tokenizer.hpp"
#include "training_dataset.hpp"
#include "transform_layer.hpp"
#include "transformer_input.hpp"
 
enum class AIResponseType { ACTION, TEXT };

class AIRequest {
private:
  String request_text;

public:
  AIRequest() = default;

  AIRequest(const char *text) : request_text(text) {}

  const String &text() const { return request_text; }
};

class AIResponse {
protected:
  String response_text;
  AIResponseType response_type;
  IntentClassifier::Intent intent;
  // FIX: was bare "Entity" with no such type in scope (wouldn't compile
  // without a stray global `using`), and was never actually assigned by
  // any constructor below - always whatever the default member
  // initializer left it as. Now properly typed and threaded through
  // every constructor so a response actually carries its predicted
  // entity.
  IntentClassifier::Entity response_entity;
  bool success;

public:
  AIResponse()
      : response_type(AIResponseType::TEXT),
        intent(IntentClassifier::Intent::UNKNOWN),
        response_entity(IntentClassifier::Entity::NONE), success(false) {}

  AIResponse(const char *text, AIResponseType type,
             IntentClassifier::Intent response_intent,
             IntentClassifier::Entity entity_value, bool successful)
      : response_text(text), response_type(type), intent(response_intent),
        response_entity(entity_value), success(successful) {}

  const String &text() const { return response_text; }

  AIResponseType type() const { return response_type; }

  IntentClassifier::Intent get_intent() const { return intent; }

  IntentClassifier::Entity get_entity() const { return response_entity; }

  const char *intent_name() const {
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

  const char *entity_name() const {
    switch (response_entity) {
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

  bool succeeded() const { return success; }
};

class AITextResponse : public AIResponse {
public:
  AITextResponse(
      const char *text,
      IntentClassifier::Intent intent = IntentClassifier::Intent::UNKNOWN,
      IntentClassifier::Entity entity = IntentClassifier::Entity::NONE,
      bool successful = true)
      : AIResponse(text, AIResponseType::TEXT, intent, entity, successful) {}
};

class AIActionResponse : public AIResponse {
public:
  AIActionResponse(const char *text, IntentClassifier::Intent intent,
                   IntentClassifier::Entity entity, bool successful = true)
      : AIResponse(text, AIResponseType::ACTION, intent, entity, successful) {}
};

// What classify() hands back to the caller: intent and entity are
// predicted independently (two separate heads over the same pooled
// trunk output), so they're bundled together here rather than one
// being derived from the other.
struct IntentEntityPrediction {
  IntentClassifier::Intent intent;
  IntentClassifier::Entity entity;
};

class RockAI {
private:
  Vocabulary &vocab;
  Tokenizer tokenizer;
  // FIX: TrainingDataset (training_dataset.hpp) isn't a template - this
  // used to be instantiated as TrainingDataset<IntentClassifier::Intent>,
  // which wouldn't compile against that class's actual (non-template)
  // definition.
  TrainingDataset dataset;

  Embedding embedding;
  PositionalEncoder pos_encoder;
  TransformerInput transformer_input;

  TransformerLayer layer;
  Vector<TransformerLayer *> layers;

  IntentClassifier intent_classifier;
  EntityClassifier entity_classifier;

  AIRequest current_request;
  AIResponse current_response;

  bool loaded = false;

  IntentEntityPrediction classify(const String &text) {
    Vector<Token> &tokens = tokenizer.tokenize(text);

    if (tokens.size() == 0)
      return {IntentClassifier::Intent::UNKNOWN,
              IntentClassifier::Entity::NONE};

    transformer_input.build(tokens);

    Vector<Vector<float>> sequence = transformer_input.get();

    for (int i = 0; i < layers.size(); i++)
      sequence = layers[i]->forward(sequence);

    // Both heads read the same final trunk sequence independently.
    IntentClassifier::Intent predicted_intent = intent_classifier.predict(sequence);
    IntentClassifier::Entity predicted_entity = entity_classifier.predict(sequence);

    return {predicted_intent, predicted_entity};
  }

  AIResponse generate_response(IntentClassifier::Intent intent,
                               IntentClassifier::Entity entity) {

    switch (intent) {

    case IntentClassifier::Intent::GREETING:
      return AITextResponse("Yo! What's up?", intent, entity);

    case IntentClassifier::Intent::MEMORY_USAGE:
      return AIActionResponse("", intent, entity);

    case IntentClassifier::Intent::TIME:
      return AIActionResponse("", intent, entity);

    case IntentClassifier::Intent::UPTIME:
      return AIActionResponse("", intent, entity);

    case IntentClassifier::Intent::OPEN_APP:
      return AIActionResponse("", intent, entity);

    case IntentClassifier::Intent::CLOSE_WINDOW:
      return AIActionResponse("", intent, entity);

    case IntentClassifier::Intent::MINIMIZE_WINDOW:
      return AIActionResponse("", intent, entity);

    case IntentClassifier::Intent::HELP:
      return AITextResponse("I can help you control Rock OS.", intent, entity);

    default:
      return AITextResponse(
          "I don't understand that yet. Still learning, lil guy", intent,
          entity, false);
    }
  }

public:
  RockAI(Vocabulary &vocabulary)
      : vocab(vocabulary), tokenizer(vocab), dataset(tokenizer, vocab),
        embedding(ROCK_AI_WEIGHTS_VOCABULARY_SIZE),
        transformer_input(embedding, pos_encoder) {

    dataset.register_examples();

    if (vocab.size() != static_cast<int>(ROCK_AI_WEIGHTS_VOCABULARY_SIZE)) {
      return;
    }

    layers.push_back(&layer);

    loaded = RockAIModel::load(embedding, layers, intent_classifier,
                               entity_classifier);
  }

  bool is_ready() const { return loaded; }

  void send_request(const AIRequest &request) {
    current_request = request;

    if (!loaded) {
      current_response =
          AITextResponse("Rock AI isn't loaded.",
                         IntentClassifier::Intent::UNKNOWN,
                         IntentClassifier::Entity::NONE, false);

      return;
    }

    IntentEntityPrediction prediction = classify(request.text());

    current_response = generate_response(prediction.intent, prediction.entity);
  }

  void send_request(const char *text) {
    AIRequest request(text);
    send_request(request);
  }

  const AIRequest &get_request() const { return current_request; }

  const AIResponse &get_response() const { return current_response; }
};
