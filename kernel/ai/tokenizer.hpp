#pragma once
#include "../containers/string.hpp"
#include "../containers/vector.hpp"
#include "token.hpp"

#include "vocabulary.hpp"

static constexpr int UNKNOWN_TOKEN_ID = -1;

enum class AIIntent {
  UNKNOWN,

  GREETING,
  MEMORY_USAGE,
  TIME,
  UPTIME,

  OPEN_APP,
  CLOSE_WINDOW,
  MINIMIZE_WINDOW,

  HELP
};

/* 
 *enum class AIIntent {
  UNKNOWN,

  // Conversation
  GREETING,
  GOODBYE,
  THANKS,
  HELP,

  // Launching / controlling things
  OPEN_APP,
  CLOSE_APP,
  SWITCH_APP,

  // Files
  FIND_FILE,
  OPEN_FILE,
  CREATE_FILE,
  DELETE_FILE,
  RENAME_FILE,
  MOVE_FILE,

  // System information
  MEMORY_USAGE,
  CPU_USAGE,
  DISK_USAGE,
  SYSTEM_INFO,
  UPTIME,
  TIME,

  // Desktop
  CHANGE_WALLPAPER,
  OPEN_SETTINGS,

  // Media
  PLAY_MUSIC,
  PAUSE_MUSIC,
  NEXT_TRACK,
  PREVIOUS_TRACK,
  VOLUME_UP,
  VOLUME_DOWN,
  MUTE,

  // Useful OS actions
  TAKE_SCREENSHOT,
  LOCK_SCREEN,
  SHUTDOWN,
  RESTART,

  // Search
  SEARCH_FILES,
  SEARCH_SYSTEM,

  // Confirmation
  CONFIRM,
  CANCEL
};
 * */

class Tokenizer {
private:
  Vector<Token> tokens;
  Vocabulary &vocab;

  static constexpr const char *punctuation_marks =
      "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";

  bool is_punctuation_mark(char c) const {
    for (int i = 0; punctuation_marks[i] != '\0'; i++) {
      if (c == punctuation_marks[i])
        return true;
    }

    return false;
  }

public:
  Tokenizer(Vocabulary &vocab) : vocab(vocab) {}

  Vector<Token> &tokenize(const String &phrase) {
    tokens.clear();

    int start = -1;

    for (int i = 0; i <= phrase.length(); i++) {
      bool end_of_phrase = i == phrase.length();
      bool separator = !end_of_phrase &&
                       (phrase[i] == ' ' || is_punctuation_mark(phrase[i]));

      if (start == -1) {
        if (!separator && !end_of_phrase)
          start = i;

        continue;
      }

      if (separator || end_of_phrase) {
        tokens.push_back(Token{phrase.substr(start, i - start)});
        start = -1;
      }
    }

    for (int i = 0; i < tokens.size(); i++) {
      VocabWordIdRetreivalEvent ev = vocab.get_word_id(tokens[i].content);

      tokens[i].id = ev.id;
    }

    return tokens;
  }

  Vector<Token> &get_current_tokens() { return tokens; }
};
