#pragma once

#include "../../data/system.hpp"

#include "../../utils/math_utils.hpp"
#include "../../utils/terminal_utils.hpp"
#include "icommand.hpp"

class CalcCommand : public ICommand {

public:
  const char *name() const { return "calc"; }

  CommandResult execute(int argc, char **argv) {
    int expr_res;

    if (argc < 2 || !argv[1])
      return CommandResult(Generator::random_phrase(foolish_phrases),
                           Colors::RED);

    if (!MathUtils::resolve_expr(argc, argv, expr_res))
      return CommandResult(Generator::random_phrase(foolish_phrases),
                           Colors::RED);

    char hex[11];
    MathUtils::int_to_hex(expr_res, hex);

    char buf[256];
    StringUtils::snprintf(buf, sizeof(buf), "ans = %s = %d", hex, expr_res);

    return CommandResult(buf, Colors::GOLD);
  }
};
