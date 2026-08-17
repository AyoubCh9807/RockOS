#pragma once
#include "../../core/rtc.hpp"
#include "../../data/system.hpp"

#pragma once

#include "icommand.hpp"

class DateCommand : public ICommand {

public:
  const char *name() const { return "date"; }

  CommandResult execute(int argc, char **argv) {

    return CommandResult(
        StringUtils::format("%s %s",
                            Generator::random_phrase(date_success_phrases),
                            RTC::get_full_time().c_str()),
        Colors::pick_random_color());
  }
};
