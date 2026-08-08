#pragma once

#include "../../core/timer.hpp"
#include "icommand.hpp"

class UptimeCommand : public ICommand {

public:
  const char *name() const override { return "uptime"; }

 String execute(int argc, char **argv) override {

    static char buffer[64];

    return Timer::get_formatted_time();
  }
};
