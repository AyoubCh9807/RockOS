#include "kernel.hpp"
#include "../../boot/graphics.hpp"
#include "../../boot/multiboot2.hpp"
#include "../memory/heap.hpp"
// #include "../process/process_manager.hpp"
// #include "../process/scheduler.hpp"
#include "../random/random.hpp"
#include "../shared/types.hpp"
#include "../shell/shell.hpp"
#include "../shell/terminal.hpp"
#include "../shell/terminal_registry.hpp"
#include "crti.hpp"
#include "timer.hpp"

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

  /*
   * Filesystem
   */
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

  /*
   * Shell
   */
  u32 current_dir = ROOT_INODE;

  TerminalUtils terminal_utils;
  Environment env(terminal_utils);

  TerminalRegistry reg(terminal_utils, fs, current_dir, env);

  Terminal terminal(terminal_utils, fs, reg, env);

  terminal.fill_registry();

  Graphics::clear(0x00FF00);

  terminal_utils.print(Generator::random_phrase(reboot_phrases), 0xFFFFFF);

  /*
   * Process system
   */
  /*  constexpr u32 TEST_MEMORY = 128 * 1024 * 1024;

    FrameAllocator frame_allocator(TEST_MEMORY);

    PageTable::debug_kernel_pml4();

    ProcessManager process_manager(frame_allocator);
    Scheduler scheduler(process_manager);

    Process *p1 = process_manager.create_process(64 * 1024, test_process_1).p;

    Process *p2 = process_manager.create_process(64 * 1024, test_process_2).p;

    if (!p1 || !p2) {
      Debugger::log("PROCESS CREATION FAILED\n");

      while (true)
        Kernel::halt();
    }

    Debugger::log("P1 CREATED\n");
    Debugger::log("P2 CREATED\n");
  */
  /*
   * Shell
   */
  ShellHistory sh;
  Shell shell(terminal, sh);

  shell.run();
}
