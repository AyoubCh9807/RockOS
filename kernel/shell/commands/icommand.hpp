#pragma once
#include "../../containers/string.hpp"
#include "../../data/shell_commands.hpp"
#include "../../tyrant_generator/generator.hpp"
#include "../../utils/terminal_utils.hpp"
#include "../../data/colors.hpp"

struct CommandResult {
  String output;
  u32 color;

  CommandResult(String output, u32 color) : output(output), color(color) {};
  CommandResult(String output) : output(output), color(Colors::WHITE) {}

};

class ICommand {
public:
  virtual const char *name() const = 0;
  virtual CommandResult execute(int argc, char **argv) = 0;
  virtual ~ICommand() = default;
};
