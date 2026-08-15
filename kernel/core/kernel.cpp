#include "kernel.hpp"
#include "../memory/memory.hpp"
#include "../multiboot/multiboot_info.hpp"
#include "../random/random.hpp"
#include "../shared/types.hpp"
#include "../shell/shell.hpp"
#include "../shell/terminal.hpp"
#include "../shell/terminal_registry.hpp"
#include "crti.hpp"
#include "timer.hpp"

// kernel/core/kernel.cpp
//   MultibootInfo(const MultibootInfoRaw* info) {

extern "C" void kernel_main(/*u32 *mb_info*/) {
  init_heap();
  call_constructors();
  Timer::init();
  Random::init();

  /* auto *info = reinterpret_cast<MultibootInfoRaw *>(mb_info);
  if (info) {
    MultibootInfo multiboot_info(info);
    TerminalUtils::print(StringUtils::format("FRAMEBUFFER WIDTH (casted to int): %d", multiboot_info.get_width()));
    TerminalUtils::print(StringUtils::format("FRAMEBUFFER HEIGHT (casted to int): %d", multiboot_info.get_height()));
    TerminalUtils::print(StringUtils::format("FRAMEBUFFER PITCH (casted to int): %d", multiboot_info.get_pitch()));
    TerminalUtils::print(StringUtils::format("FRAMEBUFFER BPP (casted to int): %d", multiboot_info.get_pitch()));
  } else {
    TerminalUtils::print("Damian could not command the framebuffer pointer to "
                         "stay still, PROCEEDING!");
  } */

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

  Environment env;
  TerminalRegistry reg(fs, current_dir, env);
  Terminal terminal(fs, reg, env);
  terminal.fill_registry();

  TerminalUtils::print(Generator::random_phrase(reboot_phrases));

  Shell shell(terminal);
  shell.run();

  while (1) {
    Kernel::halt();
  }
}
