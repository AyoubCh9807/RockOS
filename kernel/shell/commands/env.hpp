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

  String execute(int argc, char **argv) override {

    // "env" by itself just dumps everything we currently have.
    if (argc < 1 || !argv || !argv[0])
      return "Tyrant says something is wrong with that command.\n";

    if (argc == 1) {
      env.print();
      return "";
    }

    // env set NAME VALUE
    if (StringUtils::strcmp(argv[1], "set") == 0) {

      if (argc < 4 || !argv[2] || !argv[3])
        return "Damian says: env set <name> <value>\n";

      if (StringUtils::strlen(argv[2]) == 0)
        return "Tyrant says the variable needs a name.\n";

      if (StringUtils::strlen(argv[3]) == 0)
        return "Damian says an empty value is kinda useless.\n";

      if (env.set(argv[2], argv[3]))
        return Generator::random_phrase(env_set_success_phrases);

      return Generator::random_phrase(env_set_failure_phrases);
    }

    // env get NAME
    if (StringUtils::strcmp(argv[1], "get") == 0) {

      if (argc < 3 || !argv[2])
        return "Damian says: env get <name>\n";

      if (StringUtils::strlen(argv[2]) == 0)
        return "Tyrant says you forgot the variable name.\n";

      const char *value = env.get(argv[2]);

      if (StringUtils::strlen(value) == 0)
        return Generator::random_phrase(env_get_failure_phrases);

      return value;
    }

    // env unset NAME
    if (StringUtils::strcmp(argv[1], "unset") == 0) {

      if (argc < 3 || !argv[2])
        return "Damian says: env unset <name>\n";

      if (StringUtils::strlen(argv[2]) == 0)
        return "Tyrant says you can't unset nothing.\n";

      if (env.unset(argv[2]))
        return Generator::random_phrase(env_unset_success_phrases);

      return Generator::random_phrase(env_unset_failure_phrases);
    }

    // wipe the whole thing
    if (StringUtils::strcmp(argv[1], "clear") == 0) {

      if (argc != 2)
        return "Damian says: env clear\n";

      env.clear();

      return Generator::random_phrase(env_clear_success_phrases);
    }

    return "Tyrant says: env [set|get|unset|clear]\n";
  }
};
