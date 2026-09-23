#pragma once

#include "rock_ai_weights.hpp"
#include "embedding.hpp"
#include "entity_classifier.hpp"
#include "intent_classifier.hpp"
#include "transform_layer.hpp"

class RockAIModel {
public:
  static bool load(
      Embedding &embedding,
      Vector<TransformerLayer *> &layers,
      IntentClassifier &intent_classifier,
      EntityClassifier &entity_classifier) {

    if (embedding.size() !=
        static_cast<int>(ROCK_AI_WEIGHTS_VOCABULARY_SIZE))
      return false;

    if (layers.size() !=
        static_cast<int>(ROCK_AI_WEIGHTS_LAYER_COUNT))
      return false;

    int expected_parameters =
        embedding.parameter_count() +
        intent_classifier.parameter_count() +
        entity_classifier.parameter_count();

    for (int i = 0; i < layers.size(); i++)
      expected_parameters +=
          layers[i]->parameter_count();

    // NOTE: this will legitimately fail (return false) against a
    // rock_ai_weights.hpp generated before the entity classifier
    // existed, since ROCK_AI_WEIGHTS_PARAMETER_COUNT won't include its
    // weights. That's expected until you retrain and regenerate the
    // header with model_to_header - callers already treat a false
    // return here as "start from fresh weights," so this fails safe.
    if (expected_parameters !=
        static_cast<int>(ROCK_AI_WEIGHTS_PARAMETER_COUNT))
      return false;

    Vector<float> parameters;

    parameters.resize(
        ROCK_AI_WEIGHTS_PARAMETER_COUNT,
        0.0f);

    for (int i = 0;
         i < ROCK_AI_WEIGHTS_PARAMETER_COUNT;
         i++) {

      parameters[i] = ROCK_AI_WEIGHTS[i];
    }

    int cursor = 0;

    if (!embedding.import_weights(
            parameters,
            cursor))
      return false;

    for (int i = 0;
         i < layers.size();
         i++) {

      if (!layers[i]->import_weights(
              parameters,
              cursor))
        return false;
    }

    if (!intent_classifier.import_weights(
            parameters,
            cursor))
      return false;

    if (!entity_classifier.import_weights(
            parameters,
            cursor))
      return false;

    return cursor == parameters.size();
  }
};
