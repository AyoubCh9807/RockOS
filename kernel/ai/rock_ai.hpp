#pragma once

#include "../containers/vector.hpp"
#include "rock_ai_weights.hpp"

#include "embedding.hpp"
#include "intent_classifier.hpp"
// #include "model_io.hpp"
#include "positional_encoder.hpp"
#include "token.hpp"
#include "tokenizer.hpp"
#include "training_dataset.hpp"
#include "transform_layer.hpp"
#include "transformer_input.hpp"
#include "rock_ai_model.hpp"

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
  bool success;

public:
  AIResponse()
      : response_type(AIResponseType::TEXT),
        intent(IntentClassifier::Intent::UNKNOWN), success(false) {}

  AIResponse(const char *text, AIResponseType type,
             IntentClassifier::Intent response_intent, bool successful)
      : response_text(text), response_type(type), intent(response_intent),
        success(successful) {}

  const String &text() const { return response_text; }

  AIResponseType type() const { return response_type; }

  IntentClassifier::Intent get_intent() const { return intent; }

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

  bool succeeded() const { return success; }
};

class AITextResponse : public AIResponse {
public:
  AITextResponse(
      const char *text,
      IntentClassifier::Intent intent = IntentClassifier::Intent::UNKNOWN,
      bool successful = true)
      : AIResponse(text, AIResponseType::TEXT, intent, successful) {}
};

class AIActionResponse : public AIResponse {
public:
  AIActionResponse(const char *text, IntentClassifier::Intent intent,
                   bool successful = true)
      : AIResponse(text, AIResponseType::ACTION, intent, successful) {}
};

class RockAI {
private:
  Vocabulary &vocab;
  Tokenizer tokenizer;
  TrainingDataset<IntentClassifier::Intent> dataset;

  Embedding embedding;
  PositionalEncoder pos_encoder;
  TransformerInput transformer_input;

  TransformerLayer layer;
  Vector<TransformerLayer *> layers;

  IntentClassifier classifier;

  AIRequest current_request;
  AIResponse current_response;

  bool loaded = false;

  IntentClassifier::Intent classify(const String &text) {
    Vector<Token> &tokens = tokenizer.tokenize(text);

    if (tokens.size() == 0)
      return IntentClassifier::Intent::UNKNOWN;

    transformer_input.build(tokens);

    Vector<Vector<float>> sequence = transformer_input.get();

    for (int i = 0; i < layers.size(); i++)
      sequence = layers[i]->forward(sequence);

    return classifier.predict(sequence);
  }

  AIResponse generate_response(IntentClassifier::Intent intent) {

    switch (intent) {

    case IntentClassifier::Intent::GREETING:
      return AITextResponse("Yo! What's up? 🤘", intent);

    case IntentClassifier::Intent::MEMORY_USAGE:
      return AIActionResponse("You want to check memory usage.", intent);

    case IntentClassifier::Intent::TIME:
      return AIActionResponse("You want to know the time.", intent);

    case IntentClassifier::Intent::UPTIME:
      return AIActionResponse(
          "You want to know how long Rock OS has been running.", intent);

    case IntentClassifier::Intent::OPEN_APP:
      return AIActionResponse("You want to open an app.", intent);

    case IntentClassifier::Intent::CLOSE_WINDOW:
      return AIActionResponse("You want to close a window.", intent);

    case IntentClassifier::Intent::MINIMIZE_WINDOW:
      return AIActionResponse("You want to minimize a window.", intent);

    case IntentClassifier::Intent::HELP:
      return AITextResponse("I can help you control Rock OS.", intent);

    default:
      return AITextResponse("I don't understand that yet.", intent, false);
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

    loaded = RockAIModel::load(embedding, layers, classifier);
  }

  bool is_ready() const { return loaded; }

  void send_request(const AIRequest &request) {
    current_request = request;

    if (!loaded) {
      current_response = AITextResponse(
          "Rock AI isn't loaded.", IntentClassifier::Intent::UNKNOWN, false);

      return;
    }

    IntentClassifier::Intent intent = classify(request.text());

    current_response = generate_response(intent);
  }

  void send_request(const char *text) {
    AIRequest request(text);
    send_request(request);
  }

  const AIRequest &get_request() const { return current_request; }

  const AIResponse &get_response() const { return current_response; }
};
