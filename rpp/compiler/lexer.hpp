#pragma once

#include "../kernel/memory/memory.hpp"
#include "../kernel/utils/string_utils.hpp"
#include "../kernel/utils/terminal_utils.hpp"

enum class TokenType {
  ROCK,

  LPAREN,
  RPAREN,
  LBRACE,
  RBRACE,
  SEMICOLON,
  COMMA,
  SCOPE,
  DOT,

  SPACE,

  ASSIGN,

  PLUS,
  MINUS,
  DIVIDE,
  MULTIPLY,
  MODULO,

  KEYWORD,
  IDENTIFIER,

  STRING,
  INT,
  FLOAT,
  BOOLEAN,

  INVALID,
  EOF
};

struct Token {
  TokenType type;
  const char *value;
};

void print_token(Token &token) {
  // temporary debugging
  TerminalUtils::print(StringUtils::format("TYPE: %d | VALUE: %s\n",
                                           (int)token.type, token.value));
}

// Whitespace
constexpr char WHITESPACE_TAB = '\t';
constexpr char WHITESPACE_LF = '\n';
constexpr char WHITESPACE_CR = '\r';
constexpr char WHITESPACE_SPACE = ' ';

inline static bool is_alpha_char(char c) {
  return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_';
}

inline static bool is_digit_char(char c) { return c >= '0' && c <= '9'; }

inline static bool is_whitespace_char(char c) {
  return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

inline static bool is_symbol_char(char c) {
  switch (c) {
  case '(':
  case ')':
  case '{':
  case '}':
  case ';':
  case ':':
  case '.':
  case ',':
  case '=':
  case '+':
  case '-':
  case '/':
  case '*':
  case '%':
    return true;
  default:
    return false;
  }
}
inline static TokenType get_symbol_token_type(char c) {
  switch (c) {
  case '(':
    return TokenType::LPAREN;
  case ')':
    return TokenType::RPAREN;
  case '{':
    return TokenType::LBRACE;
  case '}':
    return TokenType::RBRACE;
  case ';':
    return TokenType::SEMICOLON;
  case ',':
    return TokenType::COMMA;
  case ':':
    return TokenType::SCOPE;
  case '.':
    return TokenType::DOT;
  case '=':
    return TokenType::ASSIGN;
  case '+':
    return TokenType::PLUS;
  case '-':
    return TokenType::MINUS;
  case '/':
    return TokenType::DIVIDE;
  case '*':
    return TokenType::MULTIPLY;
  case '%':
    return TokenType::MODULO;
  default:
    return TokenType::INVALID;
  }
}

// RPP keywords
constexpr const char *KEYWORDS[] = {"const", "constexpr",

                                    "i8",    "i16",       "i32", "i64",

                                    "u8",    "u16",       "u32", "u64",

                                    "f8",    "f16",       "f32", "f64",

                                    "str",   "bool",

                                    "awaken"};

bool is_keyword(const char *str) {
  if (!str || str == nullptr)
    return false;
  for (const auto &k : KEYWORDS) {
    if (StringUtils::strcmp(k, str) == 0)
      return true;
  }
  return false;
}

bool is_numeric(const char *str) {
  if (!str || str == nullptr)
    return false;

  int i = 0;
  while (str[i] != '\0') {
    if (!is_digit_char(str[i]))
      return false;
    i++;
  }
  return true;
}

static constexpr int LEFT_PAREN_CHARCODE = 40;
static constexpr int RIGHT_PAREN_CHARCODE = 41;

TokenType find_token_type(const char *str) {
  if (!str)
    return TokenType::INVALID;
  if (StringUtils::strcmp("rock", str) == 0) {
    return TokenType::ROCK;
  }
  if (is_keyword(str)) {
    return TokenType::KEYWORD;
  }
  if (is_numeric(str)) {
    return TokenType::INT;
  }
  if (is_symbol_char(str[0]) && str[1] == '\0') {
    return get_symbol_token_type(str[0]);
  }
  if (str[0] == ' ' && str[1] == '\0') {
    return TokenType::SPACE;
  }
  if (is_alpha_char(str[0])) {
    return TokenType::IDENTIFIER;
  }
  return TokenType::INVALID;
}

void LEX(const char *code) {
  Token *tokens = (Token *)kmalloc(sizeof(Token) * 1024);
  char *val = (char *)kmalloc(sizeof(char) * 32);
  if (!tokens || !val)
    return;

  if (code == nullptr || code[0] == '\0')
    return;

  int i = 0;
  int val_index = 0;
  int token_index = 0;
  Token t;
  while (code[i] != '\0') {
    if (is_whitespace_char(code[i])) {
      t.type = find_token_type(val);
      t.value = val;
      tokens[token_index++] = t;
      val_index = 0;

      i++;
      continue;
    } else if (is_alpha_char(code[i]) || is_digit_char(code[i])) {
      val[val_index++] = code[i];
      val[val_index] = '\0';
    } else if (is_symbol_char(code[i])) {
      t.type = find_token_type(val);
      t.value = val;
      tokens[token_index++] = t;
      val_index = 0;

      t.type = get_symbol_token_type(code[i]);
      t.value = &code[i];
      tokens[token_index++] = t;
    } else {
      t.type = TokenType::INVALID;
      t.value = "";
      tokens[token_index++] = t;
      val_index = 0;
    }
    i++;
  }
  t.type = find_token_type(val);
  t.value = val;
  tokens[token_index++] = t;
  val_index = 0;

  int j = 0;
}
