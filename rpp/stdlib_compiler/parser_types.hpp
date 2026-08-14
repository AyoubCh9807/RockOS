#pragma once

#include "lexer.hpp"

// DONT FORGET TO IMPLEMENT DESTRUCTORS TO PREVENT MEMORY LEAKS
// EG: delete value FOR ANYTHING W A VALUE MEMBER

struct ASTNode {
  virtual ~ASTNode() = default;
};

struct Statement : ASTNode {};
struct Expression : ASTNode {};

struct Program : ASTNode {
  std::vector<ASTNode *> body;
};

// expressions

struct Identifier : Expression {
  std::string name;
};

struct IntegerLiteral : Expression {
  int value;
};

struct FloatLiteral : Expression {
  double value;
};

struct StringLiteral : Expression {
  std::string value;
};

struct BooleanLiteral : Expression {
  bool value;
};

struct BinaryExpression : Expression {
  Expression *left;
  TokenType op;
  Expression *right;
};

struct AssignmentExpression : Expression {
  std::string name;
  std::string type;
  Expression *value;
};

// statements

struct VariableDeclaration : Statement {
  std::string type;
  std::string name;
  ASTNode *initializer;
};

struct Parameter : ASTNode {
  std::string type;
  std::string name;
};

struct Function : Statement {
  std::string return_type;
  std::string name;

  std::vector<ASTNode *> parameters;
  std::vector<ASTNode *> body;
};

struct IfStatement : Statement {
  Expression *condition;
  std::vector<ASTNode *> body;
  std::vector<ASTNode *> else_body;
};

struct WhileStatement : Statement {
  Expression *condition;
  std::vector<ASTNode *> body;
};

// Register
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

double to_double(const std::string &value) {
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

struct PrintStatement : Statement {
  Expression *value;
};
