#pragma once

#include "../../core/kernel.hpp"
#include "icommand.hpp"

class RebootCommand : public ICommand {

public:
  const char *name() const override { return "reboot"; }

  const char *execute(int argc, char **argv) override {

    Kernel::reboot();

    return "";
  }
};
