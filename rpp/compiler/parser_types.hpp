#pragma once

#include "../../kernel/containers/string.hpp"
#include "../../kernel/containers/vector.hpp"
#include "lexer.hpp"

struct ASTNode {
  virtual ~ASTNode() = default;
};

struct Statement : ASTNode {};
struct Expression : ASTNode {};

struct Program : ASTNode {
  Vector<ASTNode *> body;
};

// expressions

struct Identifier : Expression {
  String name;
};

struct IntegerLiteral : Expression {
  int value;
};

struct FloatLiteral : Expression {
  double value;
};

struct StringLiteral : Expression {
  String value;
};

struct BinaryExpression : Expression {
  ASTNode *left;
  TokenType op;
  ASTNode *right;
};

// statements

struct VariableDeclaration : Statement {
  String type;
  String name;
  ASTNode *initializer;
};

struct Parameter : ASTNode {
  String type;
  String name;
};

struct Function : Statement {
  String return_type;
  String name;

  Vector<ASTNode *> parameters;
  Vector<ASTNode *> body;
};
