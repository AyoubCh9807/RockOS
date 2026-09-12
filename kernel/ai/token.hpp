#pragma once

#include "../containers/string.hpp"

struct Token {
  String content;
  int id = 0;

  Token(String &s) : content(s) {};
  Token(String s) : content(s) {};
};
