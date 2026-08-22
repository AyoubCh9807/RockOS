#pragma once

#include "../../core/timer.hpp"
#include "icommand.hpp"

class UptimeCommand : public ICommand {

public:
  const char *name() const override { return "uptime"; }

  CommandResult execute(int argc, char **argv) override {

    char buffer[128];
    Timer::get_formatted_time_into(buffer, sizeof(buffer));

    return CommandResult(String(buffer), Colors::WHITE);
  }
};
