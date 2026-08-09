#include "kernel.hpp"
#include "../memory/memory.hpp"
#include "../random/random.hpp"
#include "../shared/types.hpp"
#include "../shell/shell.hpp"
#include "../shell/terminal.hpp"
#include "../shell/terminal_registry.hpp"
#include "crti.hpp"
#include "timer.hpp"

// kernel/core/kernel.cpp
extern "C" void kernel_main() {
  init_heap();
  call_constructors();
  Timer::init();
  Random::init();

  Disk disk;
  FileSystem fs(disk);

  Debugger::log("INODE TABLE START = ");
  Debugger::log_number(INODE_TABLE_START);
  Debugger::log("\n");

  Debugger::log("DATA BLOCK START = ");
  Debugger::log_number(DATA_BLOCK_START);
  Debugger::log("\n");

  if (!fs.mount()) {
    //    disk.test_sector_one();
    //   disk.test_sector_seven();
    Debugger::log("No filesystem, formatting...\n");
    if (!fs.format()) {
      Debugger::log("FORMAT FAILED - filesystem commands will not work\n");
    }
  } else {
    Debugger::log("MOUNT SUCCESS\n");
  }
  u32 current_dir = ROOT_INODE;

  TerminalRegistry reg(fs, current_dir);
  Terminal terminal(fs, reg);
  terminal.fill_registry();

  TerminalUtils::print(Generator::random_phrase(reboot_phrases));

  Shell shell(terminal);
  shell.run();

  while (1) {
    Kernel::halt();
  }
}
