#pragma once

#include "parser.hpp"

bool Parser::is_variable_declaration() {
  return check(TokenType::KEYWORD) &&
         check(current_position + 1, TokenType::IDENTIFIER) &&
         check(current_position + 2, TokenType::ASSIGN);
}

ASTNode *Parser::parse_variable_declaration() {
  if (!is_variable_declaration())
    return nullptr;

  VariableDeclaration *declaration =
      new VariableDeclaration();

  declaration->type =
      String(current().value.c_str());

  advance();

  declaration->name =
      String(current().value.c_str());

  advance();

  if (!expect(TokenType::ASSIGN)) {
    delete declaration;
    return nullptr;
  }

  Expression *expression = parse_expression();

  if (!expression) {
    delete declaration;
    return nullptr;
  }

  declaration->initializer = expression;

  if (!expect(TokenType::SEMICOLON)) {
    delete declaration;
    return nullptr;
  }

  return declaration;
}

ASTNode *Parser::parse_statement() {
  if (is_variable_declaration())
    return parse_variable_declaration();

  if (check(TokenType::KEYWORD) &&
      check(current_position + 1, TokenType::IDENTIFIER) &&
      check(current_position + 2, TokenType::LPAREN)) {
    return parse_function();
  }

  return nullptr;
}
