#pragma once
#include "../../data/system.hpp"
#include "../../utils/terminal_utils.hpp"

#include "icommand.hpp"

static constexpr const auto SEQUENCE_LIMIT = 10000;

class SeqCommand : public ICommand {

public:
  const char *name() const { return "seq"; }

  CommandResult execute(int argc, char **argv) {
    if (argc < 3)
      return CommandResult(Generator::random_phrase(command_not_found_phrases),
                           Colors::RED);
    int start = String(argv[1]).to_int();
    int end = String(argv[2]).to_int();
    if (start > end || end - start > SEQUENCE_LIMIT)
      return CommandResult(Generator::random_phrase(foolish_phrases),
                           Colors::RED);
    String msg = "";
    int line_length = 0;

    int columns = 20;
    if (TerminalUtils::get_global_terminal_instance()) {
      columns = TerminalUtils::get_global_terminal_instance()->get_columns();
    }

    if (argc >= 4 && StringUtils::strcmp(argv[3], "-i") == 0) {
      end += 1;
    }

    for (int i = start; i < end + 1; i++) {
      const char *num = StringUtils::iota(i);

      int num_length = StringUtils::strlen(num);

      if (line_length + num_length + 1 > columns) {
        msg += "\n";
        line_length = 0;
      }

      msg += num;
      msg += " ";

      line_length += num_length + 1;

      if (num)
        kfree((void *)num);
    }
    return CommandResult(msg, Colors::GOLD);
  }
};
