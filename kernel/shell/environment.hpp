#pragma once

#include "../containers/string.hpp"
#include "../containers/vector.hpp"
#include "../shared/types.hpp"
#include "../utils/string_utils.hpp"
#include "../utils/terminal_utils.hpp"

struct EnvVar {
  String name;
  String value;
};

class Environment {
private:
  static constexpr u32 MAX_VARS = 32;

  TerminalUtils &terminal_utils;

  Vector<EnvVar> vars;
  u32 count = 0;

public:
  Environment(TerminalUtils &utils) : terminal_utils(utils) {};

  int exists(const char *name) {
    int index = -1;
    for (int i = 0; i < vars.size(); i++) {
      if (name == vars[i].name)
        index = i;
    }
    return index;
  }
  bool set(const char *name, const char *value) {
    if (StringUtils::strlen(name) == 0) {
      // fails, handle a message or sth
    }
    int index = exists(name);
    EnvVar v;

    v.name = name;
    v.value = value;
    if (index == -1) {
      vars.push_back(v);
      return true;
    } else {
      vars[index] = v;
      return true;
    }
  };
  const char *get(const char *name) const {
    for (int i = 0; i < vars.size(); i++) {
      if (vars[i].name == name) {
        return vars[i].value.c_str();
      }
    }
    return "";
  };
  bool unset(const char *name) {
    for (int i = 0; i < vars.size(); i++) {
      if (vars[i].name == name) {
        vars.erase(i);
        return true;
      }
    }
    return false;
  };
  void print() const {
    for (int i = 0; i < vars.size(); i++) {
      terminal_utils.print_formatted(Colors::WHITE, "%s: %s\n", vars[i].name.c_str(),
                                               vars[i].value.c_str());
    }
  };

  void clear() {
    for (int i = 0; i < vars.size(); i++) {
    }
  }
};
