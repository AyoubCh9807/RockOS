#pragma once
#include "parser_types.hpp"
#include "parser.hpp"

Program *Parser::parse_program() {
  if (!expect(TokenType::ROCK))
    return INVALID_PROGRAM;

  if (!expect(TokenType::LPAREN))
    return INVALID_PROGRAM;

  if (!expect(TokenType::RPAREN))
    return INVALID_PROGRAM;

  if (!expect(TokenType::LBRACE))
    return INVALID_PROGRAM;

  Program *program = new Program();

  while (!check(TokenType::RBRACE)) {
    if (at_end()) {
      delete program;
      return INVALID_PROGRAM;
    }

    ASTNode *statement = parse_statement();

    if (!statement) {
      delete program;
      return INVALID_PROGRAM;
    }

    program->body.push_back(statement);
  }

  if (!expect(TokenType::RBRACE)) {
    delete program;
    return INVALID_PROGRAM;
  }

  if (!check(TokenType::EOF_TOKEN)) {
    delete program;
    return INVALID_PROGRAM;
  }

  return program;
}
