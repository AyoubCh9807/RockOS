#pragma once

#include "icommand.hpp"
#include "../../utils/terminal_utils.hpp"
#include "../../utils/string_utils.hpp"
#include "../../random/random.hpp"
#include "../../data/colors.hpp"

class CrystalCommand : public ICommand {
public:
    const char* name() const override { return "crystal"; }

    CommandResult execute(int argc, char** argv) override {
        auto* term = TerminalUtils::get_global_terminal_instance();
        if (!term) return CommandResult("", 0xFFFFFF);

        const char* ball[] = {
            "        .-`````````-.",
            "      .'               '.",
            "     /   .-.       .-.   \\",
            "    |   /   \\     /   \\   |",
            "    |   \\   /     \\   /   |",
            "     \\   `-'       `-'   /",
            "      '.               .'",
            "        '-...........-'",
            "            |     |",
            "            |     |",
            "            '-----'",
            nullptr
        };

        const char* roasts[] = {
            "The kernel sees your sins.",
            "Your page tables are unaligned, child.",
            "Segmentation fault in your immediate future.",
            "You will spend 3 days debugging a missing volatile.",
            "The crystal shows... a blue screen.",
            "Your heap is leaking. Your soul is leaking.",
            "Context switching? You can't even switch browser tabs.",
            "The tyrant is displeased with your brace style.",
            "You seek wisdom, but find only 0xDEADBEEF.",
            "Your next bug is behind 7 layers of abstraction.",
            "The spirits say: git push --force is a bad idea.",
            "I see... a missing semicolon. Hours of pain.",
            "Your stack is fine. Your life, however...",
            "The crystal is cloudy. Like your pointer arithmetic.",
            "You will implement a screensaver instead of fixing the scheduler.",
            "Real OS devs don't ask for ideas. They write page tables.",
            "Your bootloader is the only thing that believes in you.",
            "The gods of C++ frown upon your use of new_delete.cpp.",
            "You fear the TSS. The TSS fears nothing.",
            "Return to the matrix, neo. The scheduler can wait.",
        };
        int roast_count = sizeof(roasts) / sizeof(roasts[0]);

        const int cols = term->get_columns();

        auto next_line = [&](u32 color) {
            int col = term->get_cursor_position() % cols;
            int pad = cols - col;
            for (int i = 0; i < pad; i++)
                term->putchar(TerminalUtils::Cell(' ', color));
        };

        auto center_print = [&](const char* str, u32 color) {
            int len = 0;
            while (str && str[len]) len++;
            int pad = (cols - len) / 2;
            if (pad < 0) pad = 0;
            for (int i = 0; i < pad; i++)
                term->putchar(TerminalUtils::Cell(' ', color));
            term->print(str, color);
        };

        term->clear();

        for (int i = 0; i < 2; i++) next_line(0xFFFFFF);

        for (int i = 0; ball[i]; i++) {
            center_print(ball[i], 0x00FFFF);
            next_line(0xFFFFFF);
        }

        next_line(0xFFFFFF);
        const char* msg = roasts[Random::next() % roast_count];
        u32 glow[] = {Colors::GOLD, 0xFF00FF, 0x00FFFF, 0x00FF00, 0xFF4444};
        u32 c = glow[Random::next() % 5];

        char quoted[128];
        StringUtils::snprintf(quoted, sizeof(quoted), "\"%s\"", msg);
        center_print(quoted, c);
        next_line(0xFFFFFF);

        center_print("— The Tyrant", Colors::GRAY);
        next_line(0xFFFFFF);

        term->render();
        TerminalUtils::update_status_bar();
        return CommandResult("", 0xFFFFFF);
    }
};
