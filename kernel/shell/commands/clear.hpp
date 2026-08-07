#pragma once

#include "icommand.hpp"
#include "../../utils/terminal_utils.hpp"

class ClearCommand : public ICommand {

public:

    const char* name() const  {
        return "clear";
    }

    const char* description() const  {
        return "Clear terminal.";
    }

    const char* execute(int argc, char** argv)  {

      TerminalUtils::clear();
      return "";

    }

};
