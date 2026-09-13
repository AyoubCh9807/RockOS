#pragma once

#include "rock_ai_weights.hpp"
#include "embedding.hpp"
#include "intent_classifier.hpp"
#include "transform_layer.hpp"

class RockAIModel {
public:
  static bool load(
      Embedding &embedding,
      Vector<TransformerLayer *> &layers,
      IntentClassifier &classifier) {

    if (embedding.size() !=
        static_cast<int>(ROCK_AI_WEIGHTS_VOCABULARY_SIZE))
      return false;

    if (layers.size() !=
        static_cast<int>(ROCK_AI_WEIGHTS_LAYER_COUNT))
      return false;

    int expected_parameters =
        embedding.parameter_count() +
        classifier.parameter_count();

    for (int i = 0; i < layers.size(); i++)
      expected_parameters +=
          layers[i]->parameter_count();

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

    if (!classifier.import_weights(
            parameters,
            cursor))
      return false;

    return cursor == parameters.size();
  }
};
