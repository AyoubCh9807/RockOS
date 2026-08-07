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
			int index = (int)(Random::next() % METAL_WORD_COUNT);

			// Safe appends (bounded by sizeof(big_str))
			StringUtils::append(big_str, sizeof(big_str), metal_word_list[index]);
			StringUtils::append(big_str, sizeof(big_str), " ");

			// Put a newline after every 7 words (7, 14, 21, 28)
			if ((i + 1) % 7 == 0) {
				StringUtils::append(big_str, sizeof(big_str), "\n");
			}
		}

		StringUtils::append(big_str, sizeof(big_str), "\n");
		return big_str;
	}
};
