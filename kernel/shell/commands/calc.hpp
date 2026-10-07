#pragma once

#include "../../data/system.hpp"

#include "../../utils/math_utils.hpp"
#include "../../utils/terminal_utils.hpp"
#include "icommand.hpp"

class CalcCommand : public ICommand {

private:
  static constexpr int MAX_NUMS = 32;

  static bool is_op(const char *s) {
    return s && s[1] == '\0' &&
           (s[0] == '+' || s[0] == '-' || s[0] == '*' || s[0] == '/');
  }

public:
  const char *name() const { return "calc"; }

  CommandResult execute(int argc, char **argv) {

    // argv[0] = "calc", then: num op num [op num ...]  => argc is even, >= 4
    if (argc < 4 || argc % 2 != 0)
      return CommandResult(Generator::random_phrase(foolish_phrases),
                           Colors::RED);

    int n = (argc - 1 + 1) / 2; // number of operands
    if (n > MAX_NUMS)
      return CommandResult("too many operands", Colors::RED);

    int nums[MAX_NUMS];
    char ops[MAX_NUMS];

    for (int i = 0; i < n; i++) {
      nums[i] = StringUtils::to_int(argv[1 + 2 * i]);

      if (i < n - 1) {
        const char *op = argv[2 + 2 * i];
        if (!is_op(op))
          return CommandResult(Generator::random_phrase(foolish_phrases),
                               Colors::RED);
        ops[i] = op[0];
      }
    }

    // resolve * and /, collect leftover + and -
    int vals[MAX_NUMS];
    char vops[MAX_NUMS];
    int m = 1;
    vals[0] = nums[0];

    for (int i = 0; i < n - 1; i++) {
      char op = ops[i];
      int b = nums[i + 1];

      if (op == '*' || op == '/') {
        if (op == '/') {
          if (b == 0)
            return CommandResult("division by zero", Colors::RED);
          if (vals[m - 1] == (-2147483647 - 1) && b == -1)
            return CommandResult("overflow", Colors::RED);
        }
        vals[m - 1] = MathUtils::expr_result(vals[m - 1], b, op);
      } else {
        vops[m - 1] = op;
        vals[m++] = b;
      }
    }

    // Pass 2: left to right for + and -
    int expr_res = vals[0];
    for (int i = 0; i < m - 1; i++)
      expr_res = MathUtils::expr_result(expr_res, vals[i + 1], vops[i]);

    char hex[11];
    MathUtils::int_to_hex(expr_res, hex);

    char buf[256];
    StringUtils::snprintf(buf, sizeof(buf), "ans = %s = %d", hex, expr_res);

    return CommandResult(buf, Colors::GOLD);
  }
};
