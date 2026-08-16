#pragma once

#include "../../data/system.hpp"
#include "../../utils/terminal_utils.hpp"
#include "../environment.hpp"
#include "icommand.hpp"

class EchoCommand : public ICommand {

private:
  Environment &env;

public:
  EchoCommand(Environment &env) : env(env) {};

  const char *name() const { return "echo"; }

  String execute(int argc, char **argv) {

    for (int i = 1; i < argc; i++) {

      if (argv[i][0] == '$') {

        const char *name = argv[i] + 1;

        int index = env.exists(name);

        if (index == -1) {
          TerminalUtils::print(
              Generator::random_phrase(echo_env_failure_phrases));
        } else {
          TerminalUtils::print(env.get(name));
        }

      } else {
        TerminalUtils::print(argv[i]);
      }

      if (i != argc - 1)
        TerminalUtils::print(" ");
    }

    TerminalUtils::print("\n");

    return "";
  }
};
