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

  if (!fs.mount()) {
    TerminalUtils::print("No filesystem, formatting...\n");
    if (!fs.format()) {
      TerminalUtils::print(
          "FORMAT FAILED - filesystem commands will not work\n");
    }
  } else {
    TerminalUtils::print("MOUNT SUCCESS\n");
  }

  u32 current_dir = ROOT_INODE;

  TerminalRegistry reg(fs, current_dir);
  Terminal terminal(fs, reg);
  terminal.fill_registry();

  Shell shell(terminal);
  shell.run();

  while (1) {
    Kernel::halt();
  }
}
