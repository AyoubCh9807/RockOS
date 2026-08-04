#include "kernel.hpp"
#include "../memory/memory.hpp"
#include "../shared/types.hpp"
#include "../shell/shell.hpp"
#include "../shell/terminal.hpp"
#include "../storage/file_system.hpp"
#include "crti.hpp"
#include "timer.hpp"

extern "C" void kernel_main() {
  init_heap();
  call_constructors();
  Timer::init();

  Disk disk;
  FileSystem fs(disk);

  fs.format();

  String sample_text = "Hello ROCK OS LOVERS";
  fs.write_file((char *)"hello.txt", (u8 *)sample_text.c_str(),
                (u32)sample_text.length() + 1);

  u8 read_buffer[512] = {0};
  fs.read_file("hello.txt", read_buffer);

  Terminal::print((const char *)read_buffer);
  Terminal::print("\n");

  Shell shell;
  shell.run();

  while (1) {
    Kernel::halt();
  }
}
