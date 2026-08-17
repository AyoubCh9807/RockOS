#pragma once

#include "../../core/kernel.hpp"
#include "../../data/shell_commands.hpp"
#include "icommand.hpp"

class RebootCommand : public ICommand {

public:
  const char *name() const override { return "reboot"; }

  CommandResult execute(int argc, char **argv) override {

    Kernel::reboot();

    return CommandResult(Generator::random_phrase(reboot_phrases));
  }
};
