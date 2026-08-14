#pragma once

#include "lexer.hpp"

// DONT FORGET TO IMPLEMENT DESTRUCTORS TO PREVENT MEMORY LEAKS

enum class ASTType {
  PROGRAM,

  VARIABLE_DECLARATION,
  IF_STATEMENT,
  WHILE_STATEMENT,
  FUNCTION,
  PRINT_STATEMENT,
  PARAMETER,

  BINARY_EXPRESSION,
  ASSIGNMENT_EXPRESSION,
  IDENTIFIER,
  INTEGER_LITERAL,
  FLOAT_LITERAL,
  STRING_LITERAL,
  BOOLEAN_LITERAL
};

class ASTNode {
public:
  ASTType type;

  ASTNode(ASTType type) : type(type) {}

  virtual ~ASTNode() = default;
};

struct Statement : ASTNode {
  Statement(ASTType type) : ASTNode(type) {}
};

struct Expression : ASTNode {
  Expression(ASTType type) : ASTNode(type) {}
};

struct Program : ASTNode {
  static constexpr ASTType TYPE = ASTType::PROGRAM;

  Vector<ASTNode *> body;

  Program() : ASTNode(TYPE) {}
};

// EXPRESSIONS

struct Identifier : Expression {
  static constexpr ASTType TYPE = ASTType::IDENTIFIER;

  String name;

  Identifier(const String &name) : Expression(TYPE), name(name) {}
};

struct IntegerLiteral : Expression {
  static constexpr ASTType TYPE = ASTType::INTEGER_LITERAL;

  int value;

  IntegerLiteral(int value) : Expression(TYPE), value(value) {}
};

struct FloatLiteral : Expression {
  static constexpr ASTType TYPE = ASTType::FLOAT_LITERAL;

  double value;

  FloatLiteral(double value) : Expression(TYPE), value(value) {}
};

struct StringLiteral : Expression {
  static constexpr ASTType TYPE = ASTType::STRING_LITERAL;

  String value;

  StringLiteral(const String &value) : Expression(TYPE), value(value) {}
};

struct BooleanLiteral : Expression {
  static constexpr ASTType TYPE = ASTType::BOOLEAN_LITERAL;

  bool value;

  BooleanLiteral(bool value) : Expression(TYPE), value(value) {}
};

struct BinaryExpression : Expression {
  static constexpr ASTType TYPE = ASTType::BINARY_EXPRESSION;

  Expression *left;
  TokenType op;
  Expression *right;

  BinaryExpression(Expression *left, TokenType op, Expression *right)
      : Expression(TYPE), left(left), op(op), right(right) {}
};

struct AssignmentExpression : Expression {
  static constexpr ASTType TYPE = ASTType::ASSIGNMENT_EXPRESSION;

  String name;
  String type;
  Expression *value;

  AssignmentExpression(const String &name, const String &type,
                       Expression *value)
      : Expression(TYPE), name(name), type(type), value(value) {}
};

// STATEMENTS

struct VariableDeclaration : Statement {
  static constexpr ASTType TYPE = ASTType::VARIABLE_DECLARATION;

  String type;
  String name;
  ASTNode *initializer;

  VariableDeclaration(const String &type, const String &name,
                      ASTNode *initializer)
      : Statement(TYPE), type(type), name(name), initializer(initializer) {}
};

struct Parameter : ASTNode {
  static constexpr ASTType TYPE = ASTType::PARAMETER;

  String type;
  String name;

  Parameter(const String &type, const String &name)
      : ASTNode(TYPE), type(type), name(name) {}
};

struct Function : Statement {
  static constexpr ASTType TYPE = ASTType::FUNCTION;

  String return_type;
  String name;

  Vector<ASTNode *> parameters;
  Vector<ASTNode *> body;

  Function(const String &return_type, const String &name)
      : Statement(TYPE), return_type(return_type), name(name) {}
};

struct IfStatement : Statement {
  static constexpr ASTType TYPE = ASTType::IF_STATEMENT;

  Expression *condition;
  Vector<ASTNode *> body;
  Vector<ASTNode *> else_body;

  IfStatement(Expression *condition) : Statement(TYPE), condition(condition) {}
};

struct WhileStatement : Statement {
  static constexpr ASTType TYPE = ASTType::WHILE_STATEMENT;

  Expression *condition;
  Vector<ASTNode *> body;

  WhileStatement(Expression *condition)
      : Statement(TYPE), condition(condition) {}
};

struct PrintStatement : Statement {
  static constexpr ASTType TYPE = ASTType::PRINT_STATEMENT;

  Expression *value;

  PrintStatement(Expression *value) : Statement(TYPE), value(value) {}
};

// CUSTOM RTTI
template <typename T> T *rock_cast(ASTNode *node) {
  if (!node)
    return nullptr;

  if (node->type != T::TYPE)
    return nullptr;

  return static_cast<T *>(node);
}

Expression *rock_cast_expression(ASTNode *node) {
  if (!node)
    return nullptr;

  switch (node->type) {
  case ASTType::BINARY_EXPRESSION:
  case ASTType::ASSIGNMENT_EXPRESSION:
  case ASTType::IDENTIFIER:
  case ASTType::INTEGER_LITERAL:
  case ASTType::FLOAT_LITERAL:
  case ASTType::STRING_LITERAL:
  case ASTType::BOOLEAN_LITERAL:
    return static_cast<Expression *>(node);

  default:
    return nullptr;
  }
}

Statement *rock_cast_statement(ASTNode *node) {
  if (!node)
    return nullptr;

  switch (node->type) {
  case ASTType::VARIABLE_DECLARATION:
  case ASTType::IF_STATEMENT:
  case ASTType::WHILE_STATEMENT:
  case ASTType::FUNCTION:
  case ASTType::PRINT_STATEMENT:
    return static_cast<Statement *>(node);

  default:
    return nullptr;
  }
}
// REGISTER

enum class Register {
  EAX,
  EBX,
  ECX,
  EDX,
  ESI,
  EDI,

  XMM0,
  XMM1,
  XMM2,
  XMM3,
  XMM4,
  XMM5,
  XMM6,
  XMM7,

  INVALID
};

double to_double(const String &value) {
  double result = 0.0;
  double divisor = 10.0;
  bool after_dot = false;

  for (char c : value) {
    if (c == '.') {
      if (after_dot)
        return 0.0;

      after_dot = true;
      continue;
    }

    if (c < '0' || c > '9')
      return 0.0;

    if (!after_dot) {
      result = result * 10.0 + (c - '0');
    } else {
      result += (c - '0') / divisor;
      divisor *= 10.0;
    }
  }

  return result;
}
