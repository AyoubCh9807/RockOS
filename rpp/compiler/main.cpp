#include "lexer_test.hpp"
#include <iostream>

int main() {
  const std::string code = "awaken Damian;"
                           "awaken Tyrant;"
                           "rock () {"
                           "    i32 x = 10;"
                           "    i32 y = x + 20;"
                           "    f64 z = 3.14 + 2.58;"
                           "}";

  std::vector<Token> tokens = LEX(code);

  for (const Token &token : tokens) {
    printf("name: %s | value: %s\n", token_type_name(token.type),
           token.value.c_str());
  }

  return 0;
}
