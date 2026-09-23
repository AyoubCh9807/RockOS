#pragma once

#include "../containers/vector.hpp"

#include "embedding.hpp"
#include "entity_classifier.hpp"
#include "intent_classifier.hpp"
#include "transform_layer.hpp"

#include <cstdint>
#include <fstream>

class ModelIO {
private:
  static constexpr uint32_t MAGIC = 0x524F434B;
  // Bumped from 1 -> 2: files now also carry EntityClassifier's weights,
  // appended after IntentClassifier's. A version-1 file will fail the
  // version check below and load() will just return false (caller falls
  // back to fresh weights), it will never be misread as version 2.
  static constexpr uint32_t VERSION = 2;

  struct Header {
    uint32_t magic;
    uint32_t version;
    uint32_t vocabulary_size;
    uint32_t layer_count;
    uint32_t parameter_count;
  };

public:
  static bool save(
      const char *filename,
      const Embedding &embedding,
      const Vector<TransformerLayer *> &layers,
      const IntentClassifier &intent_classifier,
      const EntityClassifier &entity_classifier) {

    Vector<float> parameters;

    int total_parameters =
        embedding.parameter_count() +
        intent_classifier.parameter_count() +
        entity_classifier.parameter_count();

    for (int i = 0; i < layers.size(); i++)
      total_parameters += layers[i]->parameter_count();

    parameters.reserve(total_parameters);

    embedding.export_weights(parameters);

    for (int i = 0; i < layers.size(); i++)
      layers[i]->export_weights(parameters);

    intent_classifier.export_weights(parameters);
    entity_classifier.export_weights(parameters);

    Header header;

    header.magic = MAGIC;
    header.version = VERSION;
    header.vocabulary_size = embedding.size();
    header.layer_count = layers.size();
    header.parameter_count = parameters.size();

    std::ofstream file(
        filename,
        std::ios::binary);

    if (!file)
      return false;

    file.write(
        reinterpret_cast<const char *>(&header),
        sizeof(Header));

    file.write(
        reinterpret_cast<const char *>(parameters.begin()),
        parameters.size() * sizeof(float));

    return file.good();
  }

  static bool load(
      const char *filename,
      Embedding &embedding,
      Vector<TransformerLayer *> &layers,
      IntentClassifier &intent_classifier,
      EntityClassifier &entity_classifier) {

    std::ifstream file(
        filename,
        std::ios::binary);

    if (!file)
      return false;

    Header header;

    file.read(
        reinterpret_cast<char *>(&header),
        sizeof(Header));

    if (!file)
      return false;

    if (header.magic != MAGIC)
      return false;

    if (header.version != VERSION)
      return false;

    if (header.vocabulary_size !=
        static_cast<uint32_t>(embedding.size()))
      return false;

    if (header.layer_count !=
        static_cast<uint32_t>(layers.size()))
      return false;

    int expected_parameters =
        embedding.parameter_count() +
        intent_classifier.parameter_count() +
        entity_classifier.parameter_count();

    for (int i = 0; i < layers.size(); i++)
      expected_parameters +=
          layers[i]->parameter_count();

    if (header.parameter_count !=
        static_cast<uint32_t>(expected_parameters))
      return false;

    Vector<float> parameters;

    parameters.resize(
        header.parameter_count,
        0.0f);

    file.read(
        reinterpret_cast<char *>(parameters.begin()),
        parameters.size() * sizeof(float));

    if (!file)
      return false;

    int cursor = 0;

    if (!embedding.import_weights(
            parameters,
            cursor))
      return false;

    for (int i = 0; i < layers.size(); i++) {

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
