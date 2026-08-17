#pragma once

#include "../../data/system.hpp"
#include "../environment.hpp"

#include "../../utils/terminal_utils.hpp"
#include "icommand.hpp"

class EnvCommand : public ICommand {
private:
  Environment &env;

public:
  EnvCommand(Environment &env) : env(env) {}

  const char *name() const override { return "env"; }

  CommandResult execute(int argc, char **argv) override {

    // "env" by itself just dumps everything we currently have.
    if (argc < 1 || !argv || !argv[0])
      return CommandResult(
          "Tyrant says something is wrong with that command.\n", Colors::RED);

    if (argc == 1) {
      env.print();
      return CommandResult("", Colors::CYAN);
    }

    // env set NAME VALUE
    if (StringUtils::strcmp(argv[1], "set") == 0) {

      if (argc < 4 || !argv[2] || !argv[3])
        return CommandResult("Damian says: env set <name> <value>\n",
                             Colors::RED);

      if (StringUtils::strlen(argv[2]) == 0)
        return CommandResult("Tyrant says the variable needs a name.\n",
                             Colors::RED);

      if (StringUtils::strlen(argv[3]) == 0)
        return CommandResult("Damian says an empty value is kinda useless.\n",
                             Colors::RED);

      if (env.set(argv[2], argv[3]))
        return CommandResult(Generator::random_phrase(env_set_success_phrases),
                             Colors::GREEN);

      return CommandResult(Generator::random_phrase(env_set_failure_phrases),
                           Colors::RED);
    }

    // env get NAME
    if (StringUtils::strcmp(argv[1], "get") == 0) {

      if (argc < 3 || !argv[2])
        return CommandResult("Damian says: env get <name>\n", Colors::RED);

      if (StringUtils::strlen(argv[2]) == 0)
        return CommandResult("Tyrant says you forgot the variable name.\n",
                             Colors::RED);

      const char *value = env.get(argv[2]);

      if (StringUtils::strlen(value) == 0)
        return CommandResult(Generator::random_phrase(env_get_failure_phrases),
                             Colors::RED);

      return CommandResult(value, Colors::WHITE);
    }

    // env unset NAME
    if (StringUtils::strcmp(argv[1], "unset") == 0) {

      if (argc < 3 || !argv[2])
        return CommandResult("Damian says: env unset <name>\n", Colors::RED);

      if (StringUtils::strlen(argv[2]) == 0)
        return CommandResult("Tyrant says you can't unset nothing.\n",
                             Colors::RED);

      if (env.unset(argv[2]))
        return CommandResult(
            Generator::random_phrase(env_unset_success_phrases), Colors::GREEN);

      return CommandResult(Generator::random_phrase(env_unset_failure_phrases),
                           Colors::RED);
    }

    // env clear
    if (StringUtils::strcmp(argv[1], "clear") == 0) {

      if (argc != 2)
        return CommandResult("Damian says: env clear\n", Colors::RED);

      env.clear();

      return CommandResult(Generator::random_phrase(env_clear_success_phrases),
                           Colors::GREEN);
    }

    return CommandResult("Tyrant says: env [set|get|unset|clear]\n",
                         Colors::RED);
  }
};
