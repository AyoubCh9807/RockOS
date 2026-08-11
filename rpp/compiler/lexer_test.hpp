#pragma once

#include <cctype>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

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
  EOF_TOKEN
};

struct Token {
  TokenType type;
  std::string value;
};

const char *token_type_name(TokenType type) {
  switch (type) {
  case TokenType::ROCK:
    return "ROCK";

  case TokenType::LPAREN:
    return "LPAREN";
  case TokenType::RPAREN:
    return "RPAREN";
  case TokenType::LBRACE:
    return "LBRACE";
  case TokenType::RBRACE:
    return "RBRACE";
  case TokenType::SEMICOLON:
    return "SEMICOLON";
  case TokenType::COMMA:
    return "COMMA";
  case TokenType::SCOPE:
    return "SCOPE";
  case TokenType::DOT:
    return "DOT";

  case TokenType::SPACE:
    return "SPACE";

  case TokenType::ASSIGN:
    return "ASSIGN";

  case TokenType::PLUS:
    return "PLUS";
  case TokenType::MINUS:
    return "MINUS";
  case TokenType::DIVIDE:
    return "DIVIDE";
  case TokenType::MULTIPLY:
    return "MULTIPLY";
  case TokenType::MODULO:
    return "MODULO";

  case TokenType::KEYWORD:
    return "KEYWORD";
  case TokenType::IDENTIFIER:
    return "IDENTIFIER";

  case TokenType::STRING:
    return "STRING";
  case TokenType::INT:
    return "INT";
  case TokenType::FLOAT:
    return "FLOAT";
  case TokenType::BOOLEAN:
    return "BOOLEAN";

  case TokenType::INVALID:
    return "INVALID";
  case TokenType::EOF_TOKEN:
    return "EOF";

  default:
    return "UNKNOWN";
  }
}

void print_token(const Token &token) {
  std::cout << "TYPE: " << static_cast<int>(token.type)
            << " | VALUE: " << token.value << '\n';
}

// ------------------------------------------------------------
// Character helpers
// ------------------------------------------------------------

bool is_alpha_char(char c) {
  return std::isalpha(static_cast<unsigned char>(c)) || c == '_';
}

bool is_digit_char(char c) {
  return std::isdigit(static_cast<unsigned char>(c));
}

bool is_whitespace_char(char c) {
  return std::isspace(static_cast<unsigned char>(c));
}

bool is_symbol_char(char c) {
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

// ------------------------------------------------------------
// Symbols
// ------------------------------------------------------------

TokenType get_symbol_token_type(char c) {
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

// ------------------------------------------------------------
// RPP keywords
// ------------------------------------------------------------

const std::unordered_set<std::string> KEYWORDS = {
    "const", "constexpr",

    "i8",    "i16",       "i32", "i64",

    "u8",    "u16",       "u32", "u64",

    "f8",    "f16",       "f32", "f64",

    "str",   "bool",

    "awaken"};

bool is_keyword(const std::string &str) { return KEYWORDS.contains(str); }

// ------------------------------------------------------------
// Value classification
// ------------------------------------------------------------

bool is_numeric(const std::string &str) {
  if (str.empty())
    return false;

  for (char c : str) {
    if (!is_digit_char(c))
      return false;
  }

  return true;
}

bool is_integer(const std::string &value) {
  if (value.empty())
    return false;

  for (char c : value) {
    if (!is_digit_char(c))
      return false;
  }

  return true;
}

bool is_float(const std::string &value) {
  bool found_dot = false;
  bool has_digit_before = false;
  bool has_digit_after = false;

  for (std::size_t i = 0; i < value.size(); ++i) {
    char c = value[i];

    if (is_digit_char(c)) {
      if (!found_dot)
        has_digit_before = true;
      else
        has_digit_after = true;

      continue;
    }

    if (c == '.' && !found_dot) {
      found_dot = true;
      continue;
    }

    return false;
  }

  return found_dot && has_digit_before && has_digit_after;
}

TokenType find_token_type(const std::string &str) {
  if (str.empty())
    return TokenType::INVALID;

  if (str == "rock")
    return TokenType::ROCK;

  if (is_keyword(str))
    return TokenType::KEYWORD;

  if (str == "true" || str == "false")
    return TokenType::BOOLEAN;

  if (is_integer(str))
    return TokenType::INT;
  if (is_float(str))
    return TokenType::FLOAT;

  if (str.size() == 1 && is_symbol_char(str[0]))
    return get_symbol_token_type(str[0]);

  if (str.size() == 1 && str[0] == ' ')
    return TokenType::SPACE;

  if (is_alpha_char(str[0]))
    return TokenType::IDENTIFIER;
  return TokenType::INVALID;
}

// ------------------------------------------------------------
// Lexer
// ------------------------------------------------------------

std::vector<Token> LEX(const std::string &code) {
  std::vector<Token> tokens;

  std::string value;

  for (std::size_t i = 0; i < code.size(); ++i) {
    char c = code[i];

    // ----------------------------------------------------
    // Whitespace
    // ----------------------------------------------------

    if (is_whitespace_char(c)) {
      if (!value.empty()) {
        tokens.push_back({find_token_type(value), value});

        value.clear();
      }

      // For now we simply ignore whitespace.
      //
      // If you eventually want SPACE tokens,
      // handle them here explicitly.
      continue;
    }

    // ----------------------------------------------------
    // Identifiers / numbers
    // ----------------------------------------------------

    if (is_alpha_char(c)) {
      value += c;
      continue;
    }

    else if (is_digit_char(c)) {
      value += c;

      // Check if this is a float
      if (i + 1 < code.size() && code[i + 1] == '.' && i + 2 < code.size() &&
          is_digit_char(code[i + 2])) {

        value += '.';
        i++;

        while (i + 1 < code.size() && is_digit_char(code[i + 1])) {
          value += code[++i];
        }
      }
    }

    // ----------------------------------------------------
    // Symbols
    // ----------------------------------------------------

    else if (is_symbol_char(c)) {
      // Flush whatever came before the symbol.
      if (!value.empty()) {
        tokens.push_back({find_token_type(value), value});

        value.clear();
      }

      tokens.push_back({get_symbol_token_type(c), std::string(1, c)});

      continue;
    }

    // ----------------------------------------------------
    // Unknown character
    // ----------------------------------------------------

    tokens.push_back({TokenType::INVALID, std::string(1, c)});
  }

  // Flush final value.
  if (!value.empty()) {
    tokens.push_back({find_token_type(value), value});
  }

  // EOF
  tokens.push_back({TokenType::EOF_TOKEN, ""});

  return tokens;
}
