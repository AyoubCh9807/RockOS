#pragma once

#include "lexer.hpp"
#include "parser.hpp"
#include "parser_types.hpp"

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

    literal->value = std::string(current().value.c_str());

    advance();
    return literal;
  }

  if (check(TokenType::BOOLEAN)) {
    BooleanLiteral *literal = new BooleanLiteral();
    literal->value = (current().value == "true" ? true : false);

    advance();
    return literal;
  }

  if (check(TokenType::IDENTIFIER)) {
    Identifier *identifier = new Identifier();

    identifier->name = std::string(current().value.c_str());

    advance();
    return identifier;
  }

  return nullptr;
}

bool Parser::is_expression_start() {
  return check(TokenType::INT) || check(TokenType::FLOAT) ||
         check(TokenType::STRING) || check(TokenType::BOOLEAN) ||
         check(TokenType::IDENTIFIER);
}

bool Parser::is_operator() {
  return check(TokenType::PLUS) || check(TokenType::MINUS) ||
         check(TokenType::MULTIPLY) || check(TokenType::DIVIDE) ||
         check(TokenType::MODULO) || check(TokenType::EQUAL) ||
         check(TokenType::NOT_EQUAL) || check(TokenType::LT) ||
         check(TokenType::GT) || check(TokenType::GTE) || check(TokenType::LTE);
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

  if (!is_operator())
    return left;

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

    BinaryExpression *binary = new BinaryExpression();

    binary->left = left;
    binary->op = op;
    binary->right = right;

    left = binary;
  }

  return left;
}

Expression *Parser::parse_assignment_expression() {
  if (!check(TokenType::IDENTIFIER))
    return nullptr;

  Identifier *id = new Identifier();

  id->name = current().value;

  advance();

  if (!expect(TokenType::ASSIGN)) {
    delete id;
    return nullptr;
  }

  Expression *val = parse_binary_expression();

  if (!val) {
    delete id;
    return nullptr;
  }

  AssignmentExpression *exp = new AssignmentExpression();

  exp->name = id->name;
  exp->value = val;

  delete id;

  return exp;
}

Expression *Parser::parse_expression() {
  if (check(TokenType::IDENTIFIER) &&
      check(current_position + 1, TokenType::ASSIGN)) {
    return parse_assignment_expression();
  }

  return parse_binary_expression();
}
