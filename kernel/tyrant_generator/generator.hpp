#pragma once

#include "../data/metal_words.hpp"
#include "../random/random.hpp"
#include "../utils/string_utils.hpp"

class Generator {
public:
  static const char *generate_words() {
    static char big_str[1024];

    big_str[0] = '\0';

    for (int i = 0; i < 30; i++) {
    int index = Random::next() % METAL_WORD_COUNT;

      StringUtils::append(big_str, metal_word_list[index]);

      StringUtils::append(big_str, " ");
      if(i && i % 7 == 0) StringUtils::append(big_str, "\n");
    }

    StringUtils::append(big_str, "\n");

    return big_str;
  }
};
