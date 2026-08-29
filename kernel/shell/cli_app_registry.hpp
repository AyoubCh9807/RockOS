#pragma once

#include "../utils/terminal_utils.hpp"

#include "cli_apps/cli_app.hpp"
#include "cli_apps/rtop.hpp"
#include "cli_apps/phrases.hpp"

/* Add one #include per app here as they get written, same pattern
   as terminal_registry.hpp does for commands.

   #include "cli_apps/some_app.hpp"
*/

constexpr int MAX_CLI_APPS = 64;

class CliAppRegistry {
private:
  TerminalUtils& terminal_utils;
  ICliApp *apps[MAX_CLI_APPS]{};
  int count = 0;

  RtopApp rtop;
  // THIS THE ONLY APP where the class name doesnt match the command name (phrases)
  PhraseViewerApp phrases;

public:
  CliAppRegistry(TerminalUtils& terminal_utils)
    : terminal_utils(terminal_utils), rtop(terminal_utils), phrases(terminal_utils) {}

  void register_app(ICliApp *app) { apps[count++] = app; }

  void fill_registry() {
    register_app(&rtop); 
    register_app(&phrases); 
  }

  ICliApp *find(const char *name) {
    for (int i = 0; i < count; i++) {
      if (StringUtils::strcmp(name, apps[i]->name()) == 0)
        return apps[i];
    }
    return nullptr;
  }

  int get_count() const { return count; }
};
