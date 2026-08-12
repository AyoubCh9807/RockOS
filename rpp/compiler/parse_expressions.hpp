#pragma once

#include "parser.hpp"

Expression *Parser::parse_primary() {
  if (check(TokenType::INT)) {
    IntegerLiteral *literal = new IntegerLiteral();

    literal->value = 0;

    const char *value = current().value.c_str();

    for (size_t i = 0; value[i] != '\0'; i++) {
      if (value[i] < '0' || value[i] > '9') {
        delete literal;
        return nullptr;
      }

      literal->value = literal->value * 10 + (value[i] - '0');
    }

    advance();
    return literal;
  }

  if (check(TokenType::FLOAT)) {
    FloatLiteral *literal = new FloatLiteral();

    literal->value = 0.0;

    advance();
    return literal;
  }

  if (check(TokenType::STRING)) {
    StringLiteral *literal = new StringLiteral();

    literal->value = String(current().value.c_str());

    advance();
    return literal;
  }

  if (check(TokenType::IDENTIFIER)) {
    Identifier *identifier = new Identifier();

    identifier->name = String(current().value.c_str());

    advance();
    return identifier;
  }

  return nullptr;
}

Expression *Parser::parse_expression() { return parse_primary(); }
