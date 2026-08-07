#pragma once

class ICommand {

public:
  virtual const char *name() const = 0;

  virtual const char *execute(int argc, char **argv) = 0;
};
