#pragma once

#include "../../data/system.hpp"

#include "../../utils/math_utils.hpp"
#include "../../utils/terminal_utils.hpp"
#include "icommand.hpp"

class CalcCommand : public ICommand {

public:
  const char *name() const { return "calc"; }

  CommandResult execute(int argc, char **argv) {

    // calc 5 + 3 + 7 * 8
    if (argc < 4 || argc % 2 != 0)
      return CommandResult(Generator::random_phrase(foolish_phrases),
                           Colors::RED);
    int expr_res = StringUtils::to_int(argv[1]);
    for (int i = 0; i < argc - 3; i += 2) {
      expr_res = MathUtils::expr_result(
          expr_res, StringUtils::to_int(argv[i + 3]), argv[i + 2][0]);
    }

    char hex[11];
    MathUtils::int_to_hex(expr_res, hex);

    char buf[256];
    StringUtils::snprintf(buf, sizeof(buf), "ans = %s = %s", hex,
                          StringUtils::iota(expr_res));

    return CommandResult(buf, Colors::GOLD);
  }
};
