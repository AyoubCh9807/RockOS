#include "lexer.hpp"
#include "parser.hpp"
#include "code_generator.hpp"
#include "code_generator_helpers.hpp"
#include "parse_expressions.hpp"
#include "parse_program.hpp"
#include "parse_statements.hpp"
#include "parser_types.hpp"

#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "usage: rpp <file.rpp>\n";
        return 1;
    }

    const char *filename = argv[1];

    std::ifstream file(filename);

    if (!file) {
        std::cerr << "error: could not open " << filename << "\n";
        return 1;
    }

    std::string std_code(
        (std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>()
    );

    std::string code = std_code.c_str();

    // Lexer
    std::vector<Token> tokens = LEX(code);

    for(const auto& token : tokens) {
      std::cout << "Token: " << token_type_name(token.type) << std::endl;
    }

    // Parser
    Parser parser(tokens);
    Program *program = parser.parse_program();

    if (!program) {
        std::cerr << "error: parsing failed because its not a program, !program evaluated to true\n";
        return 1;
    }

    // Code generation
    CodeGenerator generator;
    std::string assembly = generator.generate(program);

    // Output assembly
    std::ofstream out("main.asm");

    if (!out) {
        std::cerr << "error: could not create main.asm\n";
        return 1;
    }

    out << assembly.c_str();

    std::cout << "Generated main.asm\n";

    return 0;
}
