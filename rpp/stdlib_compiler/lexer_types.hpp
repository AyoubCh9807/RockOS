#include "compiler_types.hpp"


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

  EQUAL,
  NOT_EQUAL,
  LT,
  GT,
  GTE,
  LTE,

  KEYWORD,
  IDENTIFIER,

  STRING,
  INT,
  FLOAT,
  BOOLEAN,

  IF,
  ELSE,
  WHILE,

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

  case TokenType::EQUAL:
    return "EQUAL";

  case TokenType::NOT_EQUAL:
    return "NOT_EQUAL";

  case TokenType::LT:
    return "LT";

  case TokenType::GT:
    return "GT";

  case TokenType::GTE:
    return "GTE";

  case TokenType::LTE:
    return "LTE";

  case TokenType::IF:
    return "IF";

  case TokenType::ELSE:
    return "ELSE";

  case TokenType::WHILE:
    return "WHILE";
  default:
    return "UNKNOWN";
  }
}


