#pragma once
#include "../../data/fortune.hpp"

#pragma once

#include "../../utils/terminal_utils.hpp"
#include "icommand.hpp"

class HeaptestCommand : public ICommand {

public:
  const char *name() const { return "heaptest"; }

  CommandResult execute(int argc, char **argv) {

    heap.flood();
    return CommandResult("Damian became too big he flooded the ENTIRE heap!", Colors::pick_random_color());
  }
};

