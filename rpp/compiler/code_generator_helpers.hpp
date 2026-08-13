#pragma once

#include "code_generator.hpp"
#include "parser_types.hpp"
#include "register_manager.hpp"

void CodeGenerator::emit(const String &line) { output += line + '\n'; }

String CodeGenerator::register_name(Register reg) {
  switch (reg) {
  case Register::EAX:
    return "EAX";

  case Register::EBX:
    return "EBX";

  case Register::ECX:
    return "ECX";

  case Register::EDX:
    return "EDX";

  case Register::ESI:
    return "ESI";

  case Register::EDI:
    return "EDI";

  default:
    return "INVALID";
  }
}

const char *CodeGenerator::binary_op(TokenType op) {
  switch (op) {
  case TokenType::PLUS:
    return "add";

  case TokenType::MINUS:
    return "sub";

  case TokenType::MULTIPLY:
    return "imul";

  case TokenType::DIVIDE:
    return "idiv";

  case TokenType::MODULO:
    return nullptr;

  default:
    return nullptr;
  }
}

const char *CodeGenerator::compare_jump(TokenType op) {
  switch (op) {
  case TokenType::EQUAL:
    return "je";

  case TokenType::NOT_EQUAL:
    return "jne";

  case TokenType::LT:
    return "jl";

  case TokenType::GT:
    return "jg";

  case TokenType::LTE:
    return "jle";

  case TokenType::GTE:
    return "jge";

  default:
    return nullptr;
  }
}

bool CodeGenerator::is_literal(Expression *expr) {
  return dynamic_cast<IntegerLiteral *>(expr) ||
         dynamic_cast<FloatLiteral *>(expr) ||
         dynamic_cast<StringLiteral *>(expr) ||
         dynamic_cast<BooleanLiteral *>(expr);
}

bool CodeGenerator::is_binary(Expression *expr) {
  return dynamic_cast<BinaryExpression *>(expr) != nullptr;
}

bool CodeGenerator::is_assignment(Expression *expr) {
  return dynamic_cast<AssignmentExpression *>(expr) != nullptr;
}

bool CodeGenerator::is_identifier(Expression *expr) {
  return dynamic_cast<Identifier *>(expr) != nullptr;
}

Register CodeGenerator::gen_expression(Expression *expr) {
  if (!expr)
    return Register::INVALID;

  if (auto *binary = dynamic_cast<BinaryExpression *>(expr))
    return gen_binary(binary);

  if (auto *assignment = dynamic_cast<AssignmentExpression *>(expr))
    return gen_assignment(assignment);

  if (auto *id = dynamic_cast<Identifier *>(expr))
    return gen_identifier(id);

  if (auto *integer = dynamic_cast<IntegerLiteral *>(expr))
    return gen_integer(integer);

  if (auto *floating = dynamic_cast<FloatLiteral *>(expr))
    return gen_float(floating);

  if (auto *string = dynamic_cast<StringLiteral *>(expr))
    return gen_string(string);

  if (auto *boolean = dynamic_cast<BooleanLiteral *>(expr))
    return gen_boolean(boolean);

  return Register::INVALID;
}

Register CodeGenerator::gen_binary(BinaryExpression *expr) {
  if (!expr)
    return Register::INVALID;

  Register left = gen_expression(expr->left);

  if (left == Register::INVALID)
    return Register::INVALID;

  Register right = gen_expression(expr->right);

  if (right == Register::INVALID) {
    registers.free(left);
    return Register::INVALID;
  }

  const char *op = binary_op(expr->op);

  if (!op) {
    registers.free(left);
    registers.free(right);
    return Register::INVALID;
  }

  emit(StringUtils::format("%s %s, %s", op, register_name(left).c_str(),
                           register_name(right).c_str()));

  // Right hand value is no longer needed.
  registers.free(right);

  // Result remains in left.
  return left;
}

Register CodeGenerator::gen_declaration(VariableDeclaration *decl) {

  if (!decl || !decl->initializer)
    return Register::INVALID;
  Expression *initializer = dynamic_cast<Expression *>(decl->initializer);

  if (!initializer)
    return Register::INVALID;

  Register reg = gen_expression(initializer);

  if (reg == Register::INVALID)
    return Register::INVALID;

  emit(StringUtils::format("mov [%s], %s", decl->name.c_str(),
                           register_name(reg).c_str()));

  registers.free(reg);

  return Register::INVALID;
}

void CodeGenerator::gen_statement(ASTNode *node) {
  if (!node)
    return;

  if (auto *expr = dynamic_cast<Expression *>(node)) {
    Register result = gen_expression(expr);

    if (result != Register::INVALID)
      registers.free(result);

    return;
  }

  if (auto *decl = dynamic_cast<VariableDeclaration *>(node)) {
    gen_declaration(decl);
    return;
  }

  if (auto *if_stmt = dynamic_cast<IfStatement *>(node)) {
    gen_if(if_stmt);
    return;
  }

  if (auto *while_stmt = dynamic_cast<WhileStatement *>(node)) {
    gen_while(while_stmt);
    return;
  }
}

Register CodeGenerator::gen_assignment(AssignmentExpression *expr) {
  if (!expr)
    return Register::INVALID;

  Register value = gen_expression(expr->value);

  if (value == Register::INVALID)
    return Register::INVALID;

  emit(StringUtils::format("mov [%s], %s", expr->name.c_str(),
                           register_name(value).c_str()));

  return value;
}

Register CodeGenerator::gen_integer(IntegerLiteral *expr) {
  if (!expr)
    return Register::INVALID;

  Register reg = registers.alloc();

  if (reg == Register::INVALID)
    return Register::INVALID;

  emit(StringUtils::format("mov %s, %d", register_name(reg).c_str(),
                           expr->value));

  return reg;
}

Register CodeGenerator::gen_identifier(Identifier *expr) {
  if (!expr)
    return Register::INVALID;

  Register reg = registers.alloc();

  if (reg == Register::INVALID)
    return Register::INVALID;

  emit(StringUtils::format("mov %s, [%s]", register_name(reg).c_str(),
                           expr->name.c_str()));

  return reg;
}

Register CodeGenerator::gen_float(FloatLiteral *expr) {
  if (!expr)
    return Register::INVALID;

  Register reg = registers.alloc();

  if (reg == Register::INVALID)
    return Register::INVALID;

  // TODO: emit float loading data reference.

  return reg;
}

Register CodeGenerator::gen_string(StringLiteral *expr) {
  if (!expr)
    return Register::INVALID;

  Register reg = registers.alloc();

  if (reg == Register::INVALID)
    return Register::INVALID;

  // TODO: strings should be stored in .rodata
  // and this register should receive their address.

  return reg;
}

Register CodeGenerator::gen_boolean(BooleanLiteral *expr) {
  if (!expr)
    return Register::INVALID;

  Register reg = registers.alloc();

  if (reg == Register::INVALID)
    return Register::INVALID;

  emit(StringUtils::format("mov %s, %d", register_name(reg).c_str(),
                           expr->value ? 1 : 0));

  return reg;
}
