#pragma once

#include "../../core/timer.hpp"
#include "icommand.hpp"

class UptimeCommand : public ICommand {

public:
  const char *name() const override { return "uptime"; }

 CommandResult execute(int argc, char **argv) override {

    static char buffer[64];

    return CommandResult(Timer::get_formatted_time(), Colors::WHITE);
  }
};
