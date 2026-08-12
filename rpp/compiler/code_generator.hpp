#pragma once

#include "parser_types.hpp"

class CodeGenerator {
private:
  String output;

  void emit(const String &line);

  // Operators
  const char *binary_op(TokenType op);
  const char *compare_jump(TokenType op);

  // AST helpers
  bool is_literal(Expression *expr);
  bool is_binary(Expression *expr);
  bool is_assignment(Expression *expr);
  bool is_identifier(Expression *expr);

  // Expressions
  void gen_expression(Expression *expr);
  void gen_binary(BinaryExpression *expr);
  void gen_assignment(AssignmentExpression *expr);

  // Statements
  void gen_statement(ASTNode *node);
  void gen_declaration(VariableDeclaration *decl);
  void gen_if(IfStatement *statement);
  void gen_while(WhileStatement *statement);

public:
  String generate(Program *program);
};
