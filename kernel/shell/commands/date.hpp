#pragma once
#include "../../core/rtc.hpp"
#include "../../data/system.hpp"

#pragma once

#include "../../utils/terminal_utils.hpp"
#include "icommand.hpp"

class DateCommand : public ICommand {

public:
  const char *name() const { return "date"; }

  String execute(int argc, char **argv) {

    TerminalUtils::print(StringUtils::format(
        "%s %s", Generator::random_phrase(date_success_phrases),
        RTC::get_full_time().c_str()));
    return "";
  }
};
