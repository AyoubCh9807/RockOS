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

  fs.format();
  Inode check{};

  InodeManager im_check(disk);

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

  Shell shell(terminal);
  shell.run();

  while (1) {
    Kernel::halt();
  }
}
