#pragma once
#include "../../data/system.hpp"
#include "icommand.hpp"

class WhichCommand : public ICommand {

private:
  ICommand **commands_ptr;
  int max_commands;

public:
  WhichCommand(ICommand *commands[], int max_commands)
      : commands_ptr(commands), max_commands(max_commands) {}

  const char *name() const { return "which"; }

  CommandResult execute(int argc, char **argv) {

    if (argc < 2 || !argv[1])
      return CommandResult(Generator::random_phrase(foolish_phrases));

    for (int i = 0; i < max_commands; i++) {
      if (commands_ptr[i] &&
          StringUtils::strcmp(commands_ptr[i]->name(), argv[1]) == 0) {
        char out[128];
        StringUtils::snprintf(out, sizeof(out),
                              "Damian says that %s exists on your system",
                              commands_ptr[i]->name());
        return CommandResult(out, Colors::GREEN);
      }
    }

    return CommandResult("Damian could not find that command", Colors::RED);
  }
};
