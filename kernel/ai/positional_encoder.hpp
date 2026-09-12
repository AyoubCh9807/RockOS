#pragma once

#include "../containers/vector.hpp"
#include "../utils/math_utils.hpp"

class PositionalEncoder {
private:
  static constexpr int EMBEDDING_DIMENSION = 128;

public:
  Vector<float> encode(int position) {
    Vector<float> encoding;

    for (int j = 0; j < EMBEDDING_DIMENSION; j += 2) {
      float exponent = (float)j / EMBEDDING_DIMENSION;
      float scale = MathUtils::pow(10000.0f, exponent);

      encoding.push_back(MathUtils::sin(position / scale));

      encoding.push_back(MathUtils::cos(position / scale));
    }
    return encoding;
  }

  int dimension() const { return EMBEDDING_DIMENSION; }
};
