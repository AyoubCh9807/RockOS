#pragma once
#include "../../containers/string.hpp"
#include "../../tyrant_generator/generator.hpp"
#include "../../utils/terminal_utils.hpp"

class ICommand {
public:
virtual const char *name() const = 0;
virtual String execute(int argc, char **argv) = 0;
virtual ~ICommand() = default;
};
