#include "shell_history.hpp"
#include "terminal.hpp"

class Shell {
private:
  Terminal &terminal;
  ShellHistory &history;

  static constexpr int SHELL_MAX_ARGS = 64;
  static constexpr int MAX_COMMAND_LENGTH = 256;

  static constexpr char* USER = "Elliot";
  static constexpr char* OS = "rock_os";

public:
  Shell(Terminal &terminal, ShellHistory &history)
      : terminal(terminal), history(history) {}

  CommandResult execute(const char *input) {
    if (!input)
      return CommandResult("\n");

    char cmd_copy[MAX_COMMAND_LENGTH];

    int i = 0;
    while (input[i] != '\0' && i < MAX_COMMAND_LENGTH - 1) {
      cmd_copy[i] = input[i];
      i++;
    }

    cmd_copy[i] = '\0';

    char *args[SHELL_MAX_ARGS];

    history.add(cmd_copy);

    return terminal.parse(cmd_copy, args, SHELL_MAX_ARGS);
  }

  ShellHistory &get_history() { return history; }

  void print_prompt() {
    terminal.terminal_utils.print_formatted(Colors::WHITE, "[%s@%s %s]$ ", USER,
                                            OS, terminal.get_current_path());
  }
};
