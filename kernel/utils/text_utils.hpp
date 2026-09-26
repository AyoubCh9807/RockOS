// text_utils.hpp
#pragma once

#include "../containers/string.hpp"

namespace TextUtils {

inline String wrap_text(const String &text, int max_width, int char_width) {
  if (max_width <= 0 || char_width <= 0)
    return text;

  const int max_chars = max_width / char_width;

  if (max_chars <= 0)
    return text;

  String wrapped;
  int line_length = 0;
  int word_start = 0;

  for (int i = 0; i <= text.length(); i++) {
    const bool end = i == text.length();
    const bool space = !end && text[i] == ' ';

    if (!space && !end)
      continue;

    const int word_length = i - word_start;

    if (word_length > max_chars) {
      if (line_length > 0) {
        wrapped += '\n';
        line_length = 0;
      }

      for (int j = word_start; j < i; j++) {
        wrapped += text[j];
        line_length++;

        if (line_length == max_chars && j + 1 < i) {
          wrapped += '\n';
          line_length = 0;
        }
      }
    } else {
      const int required =
          line_length == 0 ? word_length : line_length + 1 + word_length;

      if (required > max_chars && line_length > 0) {
        wrapped += '\n';
        line_length = 0;
      }

      if (line_length > 0) {
        wrapped += ' ';
        line_length++;
      }

      for (int j = word_start; j < i; j++) {
        wrapped += text[j];
        line_length++;
      }
    }

    word_start = i + 1;
  }

  return wrapped;
}

} // namespace TextUtils
