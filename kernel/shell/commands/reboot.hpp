#pragma once

#include "../../core/kernel.hpp"
#include "icommand.hpp"

class RebootCommand : public ICommand {

public:
  const char *name() const override { return "reboot"; }


  String execute(int argc, char **argv) override {

    TerminalUtils::print(Generator::random_phrase(reboot_phrases));

    Kernel::reboot();

    return "";
  }
};
