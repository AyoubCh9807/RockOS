#pragma once

#include "parser_types.hpp"
#include "register_manager.hpp"
#include "rodata_manager.hpp"

class CodeGenerator {
private:
  std::string output;

  RegisterManager registers;
  RodataManager rodata;

  size_t label_counter = 0;

  // Assembly
  void emit(const std::string &line);
  std::string register_name(Register reg);

  // Operators
  const char *binary_op(TokenType op);
  const char *float_binary_op(TokenType op);

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
  void gen_function(const Function &function);

  const char *inverse_compare_jump(TokenType op);

  void gen_condition(Expression *expr, const std::string &false_label);
  std::string new_label(const char *prefix);
public:
  std::string generate(Program *program);
};
