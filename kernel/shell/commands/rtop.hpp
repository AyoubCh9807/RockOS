#pragma once

#include "icommand.hpp"
#include "../../utils/terminal_utils.hpp"
#include "../../utils/string_utils.hpp"
#include "../../random/random.hpp"
#include "../../core/timer.hpp"
#include "../../data/colors.hpp"

class TopCommand : public ICommand {
public:
    const char* name() const override { return "top"; }

    CommandResult execute(int argc, char** argv) override {
        auto* term = TerminalUtils::get_global_terminal_instance();
        if (!term) return CommandResult("", 0xFFFFFF);

        struct Proc {
            int pid;
            const char* user;
            const char* name;
            int cpu;
            int mem;
            const char* time;
            const char* cmd;
        };

        static constexpr int MAX_PROCS = 12;
        Proc procs[MAX_PROCS] = {
            {1,    "root",   "init",        2,  12, "00:01:23", "/sbin/init"},
            {2,    "root",   "kthreadd",    0,   0, "00:00:00", "[kthreadd]"},
            {3,    "root",   "kworker/0",   1,   0, "00:00:12", "[kworker/0]"},
            {4,    "root",   "kworker/1",   0,   0, "00:00:08", "[kworker/1]"},
            {7,    "root",   "shell",       5,   8, "00:05:42", "/bin/shell"},
            {12,   "root",   "ksoftirqd",   0,   0, "00:00:03", "[ksoftirqd]"},
            {15,   "daemon", "rngd",        1,   2, "00:00:45", "/usr/sbin/rngd"},
            {23,   "root",   "jbd2",        0,   1, "00:00:02", "[jbd2/sda1]"},
            {42,   "root",   "tyrant",     87,  64, "00:42:00", "tyrant --lord"},
            {66,   "user",   "matrix",     12,  15, "00:01:10", "matrix-rain"},
            {69,   "user",   "snake",       8,  10, "00:02:22", "snake --arcade"},
            {1337, "root",   "kidle",       0,   0, "00:00:00", "[kidle_inject]"},
        };

        const u32 C_HEAD  = Colors::GOLD;
        const u32 C_TEXT  = 0xFFFFFF;
        const u32 C_LOW   = 0x00FF00;
        const u32 C_MED   = 0xFFFF00;
        const u32 C_HIGH  = 0xFF4444;
        const u32 C_GRAY  = Colors::GRAY;
        const u32 C_DGRAY = 0x888888;

        const int cols = term->get_columns();

        auto next_line = [&](u32 color) {
            int col = term->get_cursor_position() % cols;
            int pad = cols - col;
            for (int i = 0; i < pad; i++)
                term->putchar(TerminalUtils::Cell(' ', color));
        };

        for (int frame = 0; frame < 10; frame++) {
            term->clear();

            term->print_formatted(C_HEAD, "  my_os top — %d tasks", MAX_PROCS);
            next_line(C_TEXT);
            term->print("  --------------------------------------------------------------", C_GRAY);
            next_line(C_TEXT);
            term->print("  PID  USER      NAME        CPU%  MEM%  TIME     COMMAND", C_HEAD);
            next_line(C_TEXT);
            term->print("  --------------------------------------------------------------", C_GRAY);
            next_line(C_TEXT);

            for (int i = 0; i < MAX_PROCS; i++) {
                int delta = (int)(Random::next() % 9) - 4;
                procs[i].cpu += delta;
                if (procs[i].cpu < 0) procs[i].cpu = 0;
                if (procs[i].cpu > 100) procs[i].cpu = 100;
                if (procs[i].pid == 42)
                    procs[i].cpu = 65 + (Random::next() % 30);
            }

            for (int i = 0; i < MAX_PROCS; i++) {
                const Proc& p = procs[i];

                char left[80];
                StringUtils::snprintf(left, sizeof(left),
                    "  %-4d %-9s %-11s %3d%%  %3d%%  %-8s ",
                    p.pid, p.user, p.name, p.cpu, p.mem, p.time);
                term->print(left, C_TEXT);

                char cmd[32];
                StringUtils::snprintf(cmd, sizeof(cmd), "%-15s", p.cmd);
                term->print(cmd, C_TEXT);

                int col = term->get_cursor_position() % cols;
                int target = 62;
                int pad = target - col;
                if (pad < 0) pad = 0;
                for (int s = 0; s < pad; s++)
                    term->putchar(TerminalUtils::Cell(' ', C_TEXT));

                int bar_w = 10;
                int filled = (p.cpu * bar_w) / 100;
                u32 bar_c = (p.cpu < 30) ? C_LOW : (p.cpu < 70) ? C_MED : C_HIGH;

                term->putchar(TerminalUtils::Cell('[', C_TEXT));
                for (int b = 0; b < bar_w; b++) {
                    char ch = (b < filled) ? '#' : ' ';
                    u32 c = (b < filled) ? bar_c : C_TEXT;
                    term->putchar(TerminalUtils::Cell(ch, c));
                }
                term->putchar(TerminalUtils::Cell(']', C_TEXT));
                next_line(C_TEXT);
            }

            term->print("  --------------------------------------------------------------", C_GRAY);
            next_line(C_TEXT);

            char uptime[32];
            Timer::get_formatted_time_into(uptime, sizeof(uptime));
            term->print_formatted(C_DGRAY, "  uptime: %s  |  ", uptime);
            term->print_formatted(C_DGRAY, "cpu: %d%%  |  ", Timer::get_cpu_usage());
            term->print("press 'q' to quit (or wait...)", C_DGRAY);
            next_line(C_TEXT);

            term->render();
            TerminalUtils::update_status_bar();

            u32 t0 = Timer::get_ticks();
            while (Timer::get_ticks() - t0 < 20) {}
        }

        term->clear();
        term->render();
        TerminalUtils::update_status_bar();
        return CommandResult("", 0xFFFFFF);
    }
};
