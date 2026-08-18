#include "kernel.hpp"
#include "../memory/heap.hpp"
// #include "../multiboot/multiboot_info.hpp"
#include "../../boot/graphics.hpp"
#include "../../boot/multiboot2.hpp"
#include "../random/random.hpp"
#include "../shared/types.hpp"
#include "../shell/shell.hpp"
#include "../shell/terminal.hpp"
#include "../shell/terminal_registry.hpp"
#include "crti.hpp"
#include "timer.hpp"

// kernel/core/kernel.cpp
//   MultibootInfo(const MultibootInfoRaw* info) {

extern "C" void kernel_main(u32 mb_addr) {

  call_constructors();
  heap.init_heap();

  Timer::init();
  Random::init();

  Multiboot2::fill_tags(mb_addr);

  Disk disk;
  FileSystem fs(disk);

  if (!fs.mount()) {
    // OLD DISK TESTS:
    // disk.test_sector_one();
    // disk.test_sector_seven();
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
  /*
   *  TerminalRegistry(TerminalUtils &terminal_utils, FileSystem &fs,
                   u32 &current_dir, Environment &environment)

   * */
  TerminalRegistry reg(terminal_utils, fs, current_dir, env);
  Terminal terminal(terminal_utils, fs, reg, env);
  terminal.fill_registry();

  Graphics::clear(0x000000);

  //  Graphics::draw_rect(100, 100, 300, 100, 0xFF0000);

  //  Graphics::draw_line(100, 250, 400, 0x00FF00);

  //  Graphics::draw_string("ROCK OS", 100, 300, 0xFFFFFF);

  terminal_utils.print(Generator::random_phrase(reboot_phrases), 0xFFFFFF);

  ShellHistory sh;
  Shell shell(terminal, sh);
  shell.run();

  while (1) {
    Kernel::halt();
  }
}
