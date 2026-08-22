#pragma once
#include "../../core/rtc.hpp"
#include "../../data/system.hpp"

#pragma once

#include "icommand.hpp"

class DateCommand : public ICommand {

public:
  const char *name() const { return "date"; }

  CommandResult execute(int argc, char **argv) {

    char buf[64];
    RTC::get_full_time_into(buf, sizeof(buf));

    TerminalUtils::get_global_terminal_instance()->print_formatted(
        Colors::pick_random_color(),
        Generator::random_phrase(date_success_phrases), buf);

    return CommandResult("");
  }
};
