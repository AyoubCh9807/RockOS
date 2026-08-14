#pragma once

#include "lexer.hpp"
#include "parser_types.hpp"

Program *INVALID_PROGRAM = nullptr;

class Parser {
private:
  std::vector<Token> tokens;
  size_t token_count = 0;
  size_t current_position = 0;

public:
  Parser(const std::vector<Token> &token_list) : tokens(token_list) {
    token_count = tokens.size();
  }

  bool check(TokenType type) {
    if (current_position >= token_count)
      return false;

    if (tokens[current_position].type == TokenType::INVALID)
      return false;

    return tokens[current_position].type == type;
  }

  bool check(size_t index, TokenType type) {
    if (index >= token_count)
      return false;

    if (tokens[index].type == TokenType::INVALID)
      return false;

    return tokens[index].type == type;
  }

  Token &current() { return tokens[current_position]; }

  Token &peek(size_t offset = 1) {
    if (current_position + offset >= token_count)
      return tokens[token_count - 1];

    return tokens[current_position + offset];
  }

  void advance() {
    if (current_position < token_count)
      current_position++;
  }

  bool expect(TokenType type) {
    if (!check(type))
      return false;

    advance();
    return true;
  }

  bool at_end() const {
    return current_position >= token_count ||
           tokens[current_position].type == TokenType::EOF_TOKEN;
  }

  size_t position() const { return current_position; }

  bool is_variable_declaration();
  bool is_while_statement();
  bool is_if_statement();

  ASTNode *parse_statement();
  ASTNode *parse_variable_declaration();
  ASTNode *parse_function();

  bool is_expression_start();
  bool is_operator();

  bool expect_literal();
  bool expect_operator();

  TokenType parse_operator();

  Expression *parse_primary();
  Expression *parse_binary_expression();
  Expression *parse_expression();
  Expression *parse_assignment_expression();
  ASTNode *parse_print_statement();

  ASTNode *parse_if_statement();
  ASTNode *parse_while_statement();

  // Print
  bool is_print_statement();
  void gen_print(PrintStatement *statement);

  Program *parse_program();
};
