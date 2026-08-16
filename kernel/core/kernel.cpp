#include "kernel.hpp"
#include "../memory/memory.hpp"
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
  init_heap();
  call_constructors();
  Timer::init();
  Random::init();

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

  Environment env;
  TerminalRegistry reg(fs, current_dir, env);
  Terminal terminal(fs, reg, env);
  terminal.fill_registry();

  Multiboot2::print_tags(mb_addr);
  Graphics::clear(0x000000);

  Graphics::draw_rect(100, 100, 300, 100, 0xFF0000);

  Graphics::draw_line(100, 250, 400, 0x00FF00);

  Graphics::draw_string("ROCK OS", 100, 300, 0xFFFFFF);
  TerminalUtils::print(Generator::random_phrase(reboot_phrases));

  ShellHistory sh;
  Shell shell(terminal, sh);
  shell.run();

  while (1) {
    Kernel::halt();
  }
}
