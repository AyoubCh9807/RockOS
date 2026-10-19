#pragma once

#include "../../data/system.hpp"
#include "../../storage/file_system.hpp"
#include "../../storage/layout.hpp"
#include "icommand.hpp"

class FactorCommand : public ICommand {
private:  
    FileSystem &fs;
    u32 &current_dir;

public:
    FactorCommand(FileSystem &fs, u32 &current_dir)
        : fs(fs), current_dir(current_dir) {}

    const char *name() const override { return "factor"; }

    CommandResult execute(int argc, char **argv) override {
        if (argc < 2)
            return CommandResult(Generator::random_phrase(foolish_phrases), Colors::RED);

        char out[128] = "";
        int offset = 0;
        String nstr(argv[1]);
        int x = nstr.to_int();

        bool first = true;
        for (int i = 2; i <= x; i++) {
            while (x % i == 0) {
                offset += StringUtils::snprintf(out + offset, sizeof(out) - offset, "%s%d", first ? "" : " ", i);
                first = false;
                x /= i;
            }
        }

        return CommandResult(out, 0xFFFFFF);
    }
};
