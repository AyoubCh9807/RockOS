#pragma once

#include "lexer.hpp"
#include "parser.hpp"
#include "parser_types.hpp"

Expression *Parser::parse_primary() {
  if (check(TokenType::INT)) {
    int value = 0;

    const char *text = current().value.c_str();

    for (size_t i = 0; text[i] != '\0'; i++) {
      if (text[i] < '0' || text[i] > '9')
        return nullptr;

      value = value * 10 + (text[i] - '0');
    }

    advance();

    return new IntegerLiteral(value);
  }

  if (check(TokenType::FLOAT)) {
    double value = to_double(current().value);

    advance();

    return new FloatLiteral(value);
  }

  if (check(TokenType::STRING)) {
    String value = current().value;

    advance();

    return new StringLiteral(value);
  }

  if (check(TokenType::BOOLEAN)) {
    bool value = current().value == "true";

    advance();

    return new BooleanLiteral(value);
  }

  if (check(TokenType::IDENTIFIER)) {
    String name = current().value;

    advance();

    return new Identifier(name);
  }

  return nullptr;
}

bool Parser::is_expression_start() {
  return check(TokenType::INT) ||
         check(TokenType::FLOAT) ||
         check(TokenType::STRING) ||
         check(TokenType::BOOLEAN) ||
         check(TokenType::IDENTIFIER);
}

bool Parser::is_operator() {
  return check(TokenType::PLUS) ||
         check(TokenType::MINUS) ||
         check(TokenType::MULTIPLY) ||
         check(TokenType::DIVIDE) ||
         check(TokenType::MODULO) ||
         check(TokenType::EQUAL) ||
         check(TokenType::NOT_EQUAL) ||
         check(TokenType::LT) ||
         check(TokenType::GT) ||
         check(TokenType::GTE) ||
         check(TokenType::LTE);
}

bool Parser::expect_operator() {
  if (!is_operator())
    return false;

  advance();
  return true;
}

TokenType Parser::parse_operator() {
  TokenType op = current().type;
  advance();

  return op;
}

Expression *Parser::parse_binary_expression() {
  if (!is_expression_start())
    return nullptr;

  Expression *left = parse_primary();

  if (!left)
    return nullptr;

  while (is_operator()) {
    TokenType op = parse_operator();

    if (!is_expression_start()) {
      delete left;
      return nullptr;
    }

    Expression *right = parse_primary();

    if (!right) {
      delete left;
      return nullptr;
    }

    left = new BinaryExpression(left, op, right);
  }

  return left;
}

Expression *Parser::parse_assignment_expression() {
  if (!check(TokenType::IDENTIFIER))
    return nullptr;

  String name = current().value;

  advance();

  if (!expect(TokenType::ASSIGN))
    return nullptr;

  Expression *value = parse_binary_expression();

  if (!value)
    return nullptr;

  return new AssignmentExpression(name, "", value);
}

Expression *Parser::parse_expression() {
  if (check(TokenType::IDENTIFIER) &&
      check(current_position + 1, TokenType::ASSIGN)) {
    return parse_assignment_expression();
  }

  return parse_binary_expression();
}
