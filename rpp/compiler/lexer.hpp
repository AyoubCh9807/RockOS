#pragma once

#include "../../kernel/containers/string.hpp"
#include "../../kernel/containers/vector.hpp"
#include "../../kernel/utils/string_utils.hpp"
#include "../../kernel/utils/terminal_utils.hpp"

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
  String value;
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
  TerminalUtils::print(
      StringUtils::format(
          "Type: %s | Val: %s\n",
          token_type_name(token.type),
          token.value.c_str()));
}

bool is_alpha_char(char c) {
  return (c >= 'A' && c <= 'Z') ||
         (c >= 'a' && c <= 'z') ||
         c == '_';
}

bool is_digit_char(char c) {
  return c >= '0' && c <= '9';
}

bool is_whitespace_char(char c) {
  return c == ' ' ||
         c == '\t' ||
         c == '\n' ||
         c == '\r';
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

bool is_keyword(const String &str) {
  if (str == "const")
    return true;

  if (str == "constexpr")
    return true;

  if (str == "i8" || str == "i16" ||
      str == "i32" || str == "i64")
    return true;

  if (str == "u8" || str == "u16" ||
      str == "u32" || str == "u64")
    return true;

  if (str == "f8" || str == "f16" ||
      str == "f32" || str == "f64")
    return true;

  if (str == "str")
    return true;

  if (str == "bool")
    return true;

  if (str == "awaken")
    return true;

  return false;
}

bool is_integer(const String &value) {
  if (value.length() == 0)
    return false;

  for (size_t i = 0; i < (size_t)value.length(); i++) {
    if (!is_digit_char(value[i]))
      return false;
  }

  return true;
}

bool is_float(const String &value) {
  if (value.length() == 0)
    return false;

  bool found_dot = false;
  bool has_digit_before = false;
  bool has_digit_after = false;

  for (size_t i = 0; i < (size_t)value.length(); i++) {
    char c = value[i];

    if (is_digit_char(c)) {
      if (found_dot)
        has_digit_after = true;
      else
        has_digit_before = true;

      continue;
    }

    if (c == '.' && !found_dot) {
      found_dot = true;
      continue;
    }

    return false;
  }

  return found_dot &&
         has_digit_before &&
         has_digit_after;
}

TokenType find_token_type(const String &str) {
  if (str.length() == 0)
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

  if (str.length() == 1 &&
      is_symbol_char(str[0])) {
    return get_symbol_token_type(str[0]);
  }

  if (is_alpha_char(str[0]))
    return TokenType::IDENTIFIER;

  return TokenType::INVALID;
}

Vector<Token> LEX(const String &code) {
  Vector<Token> tokens;
  String value;

  size_t i = 0;

  while (i < (size_t)code.length()) {
    char c = code[i];

    if (is_whitespace_char(c)) {
      if (value.length() > 0) {
        tokens.push_back({
            find_token_type(value),
            value
        });

        value.clear();
      }

      i++;
      continue;
    }

    if (c == '"') {
      if (value.length() > 0) {
        tokens.push_back({
            find_token_type(value),
            value
        });

        value.clear();
      }

      String string_value;

      i++;

      bool closed = false;

      while (i < (size_t)code.length()) {
        char string_char = code[i];

        if (string_char == '"') {
          closed = true;
          break;
        }

        string_value += string_char;
        i++;
      }

      if (!closed) {
        tokens.push_back({
            TokenType::INVALID,
            string_value
        });

        return tokens;
      }

      tokens.push_back({
          TokenType::STRING,
          string_value
      });

      i++;
      continue;
    }

    if (is_alpha_char(c)) {
      value += c;
      i++;
      continue;
    }

    if (is_digit_char(c)) {
      value += c;
      i++;

      if (i < (size_t)code.length() &&
          code[i] == '.' &&
          i + 1 < (size_t)code.length() &&
          is_digit_char(code[i + 1])) {

        value += '.';
        i++;

        while (i < (size_t)code.length() &&
               is_digit_char(code[i])) {
          value += code[i];
          i++;
        }
      }

      continue;
    }

    if (is_symbol_char(c)) {
      if (value.length() > 0) {
        tokens.push_back({
            find_token_type(value),
            value
        });

        value.clear();
      }

      String symbol;
      symbol += c;

      tokens.push_back({
          get_symbol_token_type(c),
          symbol
      });

      i++;
      continue;
    }

    if (value.length() > 0) {
      tokens.push_back({
          find_token_type(value),
          value
      });

      value.clear();
    }

    String invalid;
    invalid += c;

    tokens.push_back({
        TokenType::INVALID,
        invalid
    });

    i++;
  }

  if (value.length() > 0) {
    tokens.push_back({
        find_token_type(value),
        value
    });
  }

  tokens.push_back({
      TokenType::EOF_TOKEN,
      String("")
  });

  return tokens;
}
