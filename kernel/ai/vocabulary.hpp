#pragma once

#include "../containers/string.hpp"
#include "../containers/vector.hpp"

struct VocabWordIdRetreivalEvent {
  int id;
  bool success;
};

struct VocabularySlot {
  String word;
  int id;
};

class Vocabulary {
private:
  Vector<VocabularySlot> words;

  static constexpr int UNKNOWN_TOKEN_ID = 0;
  int current_id = 1;

public:
  Vocabulary() {
    words.push_back(VocabularySlot{String("<UNK>"), UNKNOWN_TOKEN_ID});
  }

  void add(String &word) {
    VocabWordIdRetreivalEvent ev = get_word_id(word);

    if (!ev.success)
      words.push_back(VocabularySlot{word, current_id++});
  }

  int size() const {
    return current_id;
  }

  int unknown_token_id() const {
    return UNKNOWN_TOKEN_ID;
  }

  VocabWordIdRetreivalEvent get_word_id(String &req_word) {
    for (int i = 0; i < words.size(); i++) {
      if (words[i].word == req_word)
        return VocabWordIdRetreivalEvent{words[i].id, true};
    }

    return VocabWordIdRetreivalEvent{UNKNOWN_TOKEN_ID, false};
  }
};
