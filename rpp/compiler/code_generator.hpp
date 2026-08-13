#pragma once

#include "parser_types.hpp"
#include "register_manager.hpp"

class CodeGenerator {
private:
  String output;
  RegisterManager registers;

  // Assembly
  void emit(const String &line);
  String register_name(Register reg);

  // Operators
  const char *binary_op(TokenType op);
  const char *compare_jump(TokenType op);

  // AST helpers
  bool is_literal(Expression *expr);
  bool is_binary(Expression *expr);
  bool is_assignment(Expression *expr);
  bool is_identifier(Expression *expr);

  // Expressions
  Register gen_expression(Expression *expr);
  Register gen_binary(BinaryExpression *expr);
  Register gen_assignment(AssignmentExpression *expr);

  Register gen_identifier(Identifier *expr);
  Register gen_integer(IntegerLiteral *expr);
  Register gen_float(FloatLiteral *expr);
  Register gen_string(StringLiteral *expr);
  Register gen_boolean(BooleanLiteral *expr);

  // Statements
  void gen_statement(ASTNode *node);
  Register gen_declaration(VariableDeclaration *decl);
  void gen_if(IfStatement *statement);
  void gen_while(WhileStatement *statement);

public:
  String generate(Program *program);
};
