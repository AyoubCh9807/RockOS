#include "../../kernel/storage/file_system.hpp"
#include "../../kernel/memory/memory.hpp"

#include "lexer.hpp"
#include "lexer_types.hpp"

#include "parser.hpp"
#include "parser_types.hpp"

#include "parse_expressions.hpp"
#include "parse_statements.hpp"
#include "parse_program.hpp"

#include "register_manager.hpp"
#include "rodata_manager.hpp"

#include "code_generator.hpp"
#include "code_generator_helpers.hpp"
int compile(FileSystem& fs, char* input_path, char* output_path) {
    // Read source file
    u8 buffer[64 * 1024];
    size_t bytes_read = 0;

    if (!fs.read_file(
            input_path,
            buffer,
            sizeof(buffer),
            bytes_read,
            ROOT_INODE)) {
        return 1;
    }

    // Convert the bytes into your String
    String code;

    for (size_t i = 0; i < bytes_read; i++) {
        code += (char)buffer[i];
    }

    // Lexer
    Vector<Token> tokens = LEX(code);

    for (const auto& token : tokens) {
        TerminalUtils::print("Token: ");
        TerminalUtils::print(token_type_name(token.type));
        TerminalUtils::print("\n");
    }

    // Parser
    Parser parser(tokens);
    Program* program = parser.parse_program();

    if (!program) {
        TerminalUtils::print(
            "error: parsing failed because input is not a program\n"
        );
        return 1;
    }

    // Code generation
    CodeGenerator generator;
    String assembly = generator.generate(program);

    // Write generated assembly
    if (!fs.write_file(
            output_path,
            (const u8*)assembly.c_str(),
            assembly.length(),
            ROOT_INODE)) {
        TerminalUtils::print("error: could not write output file\n");
        return 1;
    }

    return 0;
}
