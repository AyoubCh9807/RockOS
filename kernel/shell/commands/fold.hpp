#pragma once

#include "../../data/system.hpp"
#include "../../storage/file_system.hpp"
#include "../../storage/layout.hpp"
#include "icommand.hpp"

class FoldCommand : public ICommand {
private:  
    FileSystem &fs;
    u32 &current_dir;

public:
    FoldCommand(FileSystem &fs, u32 &current_dir)
        : fs(fs), current_dir(current_dir) {}

    const char *name() const override { return "fold"; }

    CommandResult execute(int argc, char **argv) override {
        if (argc < 2)
            return CommandResult(Generator::random_phrase(foolish_phrases), Colors::RED);

        int width = 40; // Default wrap width
        int text_arg_idx = 1;

        // If the first argument is a number, treat it as the width and the next as text
        String first_arg(argv[1]);
        if (argc >= 3) {
            int parsed_width = first_arg.to_int();
            if (parsed_width > 0) {
                width = parsed_width;
                text_arg_idx = 2;
            }
        }

        // Combine any remaining arguments into a single text buffer
        char text_buf[256] = "";
        int text_offset = 0;
        for (int i = text_arg_idx; i < argc; i++) {
            text_offset += StringUtils::snprintf(text_buf + text_offset, sizeof(text_buf) - text_offset, 
                                                "%s%s", (i > text_arg_idx) ? " " : "", argv[i]);
        }

        // Wrap the text by inserting newlines every `width` characters
        char out[512] = "";
        int out_offset = 0;
        int current_line_len = 0;

        for (int i = 0; text_buf[i] != '\0'; i++) {
            char c = text_buf[i];

            if (c == '\n') {
                out_offset += StringUtils::snprintf(out + out_offset, sizeof(out) - out_offset, "\n");
                current_line_len = 0;
                continue;
            }

            if (current_line_len >= width) {
                out_offset += StringUtils::snprintf(out + out_offset, sizeof(out) - out_offset, "\n");
                current_line_len = 0;
            }

            out_offset += StringUtils::snprintf(out + out_offset, sizeof(out) - out_offset, "%c", c);
            current_line_len++;
        }

        return CommandResult(out, 0xFFFFFF);
    }
};
