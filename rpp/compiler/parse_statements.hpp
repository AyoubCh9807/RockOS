#pragma once

#include "parser.hpp"
#include "parser_types.hpp"

bool Parser::is_variable_declaration() {
  return check(TokenType::KEYWORD) &&
         check(current_position + 1, TokenType::IDENTIFIER) &&
         check(current_position + 2, TokenType::ASSIGN);
}

bool Parser::is_if_statement() { return check(TokenType::IF); }

bool Parser::is_while_statement() { return check(TokenType::WHILE); }

ASTNode *Parser::parse_variable_declaration() {
  if (!is_variable_declaration())
    return nullptr;

  VariableDeclaration *declaration = new VariableDeclaration();

  declaration->type = String(current().value.c_str());

  advance();

  declaration->name = String(current().value.c_str());

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

ASTNode *Parser::parse_if_statement() {
  if (!is_if_statement())
    return nullptr;
  // while(x > 5) {
  //    x = x + 1;
  // }

  IfStatement *statement = new IfStatement();

  if (!expect(TokenType::IF)) {
    delete statement;
    return nullptr;
  }
  if (!expect(TokenType::LPAREN)) {
    delete statement;
    return nullptr;
  }
  statement->condition = parse_expression();
  if (!statement->condition) {
    delete statement;
    return nullptr;
  }

  if (!expect(TokenType::RPAREN)) {
    delete statement;
    return nullptr;
  }
  if (!expect(TokenType::LBRACE)) {
    delete statement;
    return nullptr;
  }
  if (at_end()) {
    delete statement;
    return nullptr;
  }
  while (!check(TokenType::RBRACE)) {
    if (at_end()) {
      delete statement;
      return nullptr;
    }

    ASTNode *body_statement = parse_statement();

    if (!body_statement) {
      delete statement;
      return nullptr;
    }

    statement->body.push_back(body_statement);
  }

  if (!expect(TokenType::RBRACE)) {
    delete statement;
    return nullptr;
  }

  // check for else statement
  if (!expect(TokenType::ELSE))
    return statement;

  if (!expect(TokenType::LBRACE)) {
    delete statement;
    return nullptr;
  }

  while (!check(TokenType::RBRACE)) {
    if (at_end()) {
      delete statement;
      return nullptr;
    }

    ASTNode *else_statement = parse_statement();
    if (!else_statement) {
      delete statement;
      return nullptr;
    }
    statement->else_body.push_back(else_statement);
  }

  if (!expect(TokenType::RBRACE)) {
    delete statement;
    return nullptr;
  }

  return statement;
}
ASTNode *Parser::parse_while_statement() {
  if (!is_while_statement())
    return nullptr;
  // while(x > 5) {
  //    x = x + 1;
  // }

  WhileStatement *statement = new WhileStatement();

  if (!expect(TokenType::WHILE)) {
    delete statement;
    return nullptr;
  }
  if (!expect(TokenType::LPAREN)) {
    delete statement;
    return nullptr;
  }

  statement->condition = parse_expression();
  if (!statement->condition) {
    delete statement;
    return nullptr;
  }

  if (!expect(TokenType::RPAREN)) {
    delete statement;
    return nullptr;
  }
  if (!expect(TokenType::LBRACE)) {
    delete statement;
    return nullptr;
  }
  if (at_end()) {
    delete statement;
    return nullptr;
  }
  while (!check(TokenType::RBRACE)) {
    if (at_end()) {
      delete statement;
      return nullptr;
    }

    ASTNode *body_statement = parse_statement();

    if (!body_statement) {
      delete statement;
      return nullptr;
    }

    statement->body.push_back(body_statement);
  }

  if (!expect(TokenType::RBRACE)) {
    delete statement;
    return nullptr;
  }

  return statement;
}

ASTNode *Parser::parse_statement() {
  if (is_variable_declaration())
    return parse_variable_declaration();

  if (is_if_statement())
    return parse_if_statement();

  if (is_while_statement())
    return parse_while_statement();

  if (check(TokenType::KEYWORD) &&
      check(current_position + 1, TokenType::IDENTIFIER) &&
      check(current_position + 2, TokenType::LPAREN)) {
    return parse_function();
  }

  return nullptr;
}
