#pragma once

#include "../../storage/file_system.hpp"
#include "icommand.hpp"

class RmdirCommand : public ICommand {
  FileSystem &fs;
  u32 &current_dir;

public:
  RmdirCommand(FileSystem &fs, u32 &current_dir)
      : fs(fs), current_dir(current_dir) {}

  const char *name() const override { return "rmdir"; }

  String execute(int argc, char **argv) override {

    Debugger::log("RMDIR START\n");

    if (argc < 2)
      return Generator::random_phrase(command_not_found_phrases);

    Debugger::log("REMOVING: ");
    Debugger::log(argv[1]);
    Debugger::log("\n");
    bool result = fs.remove_directory(argv[1], current_dir);

    Debugger::log("RMDIR END\n");

    if (result)
      return Generator::random_phrase(rmdir_phrases);

    return Generator::random_phrase(command_not_found_phrases);
  }
};
