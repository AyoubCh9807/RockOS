#include "kernel.hpp"
#include "../memory/heap.hpp"
#include "../../boot/graphics.hpp"
#include "../../boot/multiboot2.hpp"
#include "../random/random.hpp"
#include "../shared/types.hpp"
#include "../shell/shell.hpp"
#include "../shell/terminal.hpp"
#include "../shell/terminal_registry.hpp"
#include "crti.hpp"
#include "timer.hpp"
#include "../process/process_manager.hpp"

extern "C" void test_process_1() {
  while (true) {
    Graphics::draw_string("1", 100, 100, 0x000000);
  }
}

extern "C" void test_process_2() {
  while (true) {
    Graphics::draw_string("2", 200, 100, 0x000000);
  }
}

extern "C" void kernel_main(u64 mb_addr) {
  call_constructors();
  heap.init_heap();

  Timer::init();
  Random::init();

  Multiboot2::fill_tags(mb_addr);

  Disk disk;
  FileSystem fs(disk);

  if (!fs.mount()) {
    Debugger::log("No filesystem, formatting...\n");

    if (!fs.format()) {
      Debugger::log("FORMAT FAILED - filesystem commands will not work\n");
    }
  } else {
    Debugger::log("MOUNT SUCCESS\n");
  }

  u32 current_dir = ROOT_INODE;

  TerminalUtils terminal_utils;
  Environment env(terminal_utils);

  TerminalRegistry reg(terminal_utils, fs, current_dir, env);
  Terminal terminal(terminal_utils, fs, reg, env);
  terminal.fill_registry();


  Graphics::clear(0xFFFFFF);

  terminal_utils.print(
      Generator::random_phrase(reboot_phrases),
      0xFFFFFF
  );

  Process *p1 =
      process_manager.create_process(64 * 1024, test_process_1);

  Process *p2 =
      process_manager.create_process(64 * 1024, test_process_2);

  if (!p1 || !p2) {
    Debugger::log("PROCESS CREATION FAILED\n");

    while (true)
      Kernel::halt();
  }

  Debugger::log("P1 CREATED\n");
  Debugger::log("P2 CREATED\n");

  while (true) {
    Kernel::halt();
  }
}
