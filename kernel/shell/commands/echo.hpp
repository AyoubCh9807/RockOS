#pragma once

#include "../../data/system.hpp"
#include "../../utils/terminal_utils.hpp"
#include "../environment.hpp"
#include "icommand.hpp"

class EchoCommand : public ICommand {

private:
  Environment &env;
  TerminalUtils &terminal_utils;

public:
  EchoCommand(Environment &env, TerminalUtils &utils)
      : env(env), terminal_utils(utils) {};

  const char *name() const { return "echo"; }

  CommandResult execute(int argc, char **argv) {

    for (int i = 1; i < argc; i++) {

      if (argv[i][0] == '$') {

        const char *name = argv[i] + 1;

        int index = env.exists(name);

        if (index == -1) {
          terminal_utils.print(
              Generator::random_phrase(echo_env_failure_phrases), 0xFFFFFF);
        } else {
          terminal_utils.print(env.get(name), 0xFFFFFF);
        }

      } else {
        terminal_utils.print(argv[i], 0xFFFFFF);
      }

      if (i != argc - 1)
        terminal_utils.print(" ", 0xFFFFFF);
    }

    terminal_utils.print("\n", 0xFFFFFF);

    return CommandResult("");
  }
};
