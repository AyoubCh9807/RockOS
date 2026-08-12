#pragma once

#include "code_generator.hpp"
#include "parser_types.hpp"

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
void CodeGenerator::emit(const String &line) { output += line + '\n'; }

void CodeGenerator::gen_expression(Expression *expr) {}

void CodeGenerator::gen_binary(BinaryExpression *expr) {
  if (!expr)
    return;

  // Generate LEFT
  if (auto *id = dynamic_cast<Identifier *>(expr->left)) {
    // identifier
    emit(StringUtils::format("mov eax, %s", id->name.c_str()));

  } else if (auto *integer = dynamic_cast<IntegerLiteral *>(expr->left)) {
    emit(StringUtils::format("mov eax, %d", expr->left));

  } else if (auto *floating = dynamic_cast<FloatLiteral *>(expr->left)) {
    // float
  } else if (auto *string = dynamic_cast<StringLiteral *>(expr->left)) {
    // string
  } else if (auto *boolean = dynamic_cast<BooleanLiteral *>(expr->left)) {
    // boolean
  } else if (auto *binary = dynamic_cast<BinaryExpression *>(expr->left)) {
    // nested binary expression
  }
  if (auto *literal = dynamic_cast<IntegerLiteral *>(expr->left)) {
  } else if (auto *id = dynamic_cast<Identifier *>(expr->left)) {
    emit(StringUtils::format("mov eax, [%s]", id->name.c_str()));
  }

  // Generate RIGHT
  if (auto *literal = dynamic_cast<IntegerLiteral *>(expr->right)) {
    emit(StringUtils::format("mov ebx, %d", literal->value));
  } else if (auto *id = dynamic_cast<Identifier *>(expr->right)) {
    emit(StringUtils::format("mov ebx, [%s]", id->name.c_str()));
  }

  emit(StringUtils::format("%s eax, ebx", binary_op(expr->op)));
}

void CodeGenerator::gen_assignment(AssignmentExpression *expr) {}

void CodeGenerator::gen_statement(ASTNode *node) {}

void CodeGenerator::gen_declaration(VariableDeclaration *decl) {}

void CodeGenerator::gen_if(IfStatement *statement) {}

void CodeGenerator::gen_while(WhileStatement *statement) {}

String CodeGenerator::generate(Program *program) {}
