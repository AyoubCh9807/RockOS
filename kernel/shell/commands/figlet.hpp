#pragma once

#include "../../data/system.hpp"
#include "../../storage/file_system.hpp"
#include "../../storage/layout.hpp"
#include "icommand.hpp"
#include "../../data/colors.hpp"

class FigletCommand : public ICommand {

public:

    const char *name() const override { return "figlet"; }

    CommandResult execute(int argc, char **argv) override {
        if (argc < 2)
            return CommandResult(Generator::random_phrase(foolish_phrases), Colors::RED);

        // Combine any arguments into a single text buffer
        char text_buf[256] = "";
        int text_offset = 0;
        for (int i = 1; i < argc; i++) {
            text_offset += StringUtils::snprintf(text_buf + text_offset, sizeof(text_buf) - text_offset, 
                                                "%s%s", (i > 1) ? " " : "", argv[i]);
        }

        // Calculate string length manually
        int len = 0;
        while (text_buf[len] != '\0') {
            len++;
        }

        // Build a stylish ASCII banner box
        char out[512] = "";
        int out_offset = 0;

        // Top border
        out_offset += StringUtils::snprintf(out + out_offset, sizeof(out) - out_offset, "+");
        for (int i = 0; i < len + 4; i++) {
            out_offset += StringUtils::snprintf(out + out_offset, sizeof(out) - out_offset, "-");
        }
        out_offset += StringUtils::snprintf(out + out_offset, sizeof(out) - out_offset, "+\n");

        // Middle line containing the text padded w spaces
        out_offset += StringUtils::snprintf(out + out_offset, sizeof(out) - out_offset, "|  %s  |\n", text_buf);

        // Bottom border
        out_offset += StringUtils::snprintf(out + out_offset, sizeof(out) - out_offset, "+");
        for (int i = 0; i < len + 4; i++) {
            out_offset += StringUtils::snprintf(out + out_offset, sizeof(out) - out_offset, "-");
        }
        out_offset += StringUtils::snprintf(out + out_offset, sizeof(out) - out_offset, "+");

        // Return with tuff and cool effect
        return CommandResult(out, Colors::GREEN);
    }
};
