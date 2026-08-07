#include "kernel.hpp"
#include "../memory/memory.hpp"
#include "../random/random.hpp"
#include "../shared/types.hpp"
#include "../shell/shell.hpp"
#include "../shell/terminal.hpp"
#include "../shell/terminal_registry.hpp"
#include "crti.hpp"
#include "timer.hpp"

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
      TerminalUtils::print("FORMAT FAILED\n");
      return;
    }
  } else {
    TerminalUtils::print("MOUNT SUCCESS\n");
  } 
  //  String sample_text = "Hello ROCK OS LOVERS";
  //  fs.write_file((char *)"hello.txt", (u8 *)sample_text.c_str(),
  //                (u32)sample_text.length() + 1);

  //  u8 read_buffer[512] = {0};
  //  fs.read_file("hello.txt", read_buffer);

  //  Terminal::print((const char *)read_buffer);
  //  Terminal::print("\n");

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
