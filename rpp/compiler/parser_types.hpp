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
enum class Register { EAX, EBX, ECX, EDX, ESI, EDI, INVALID };
