#include "kernel.hpp"
#include "../drivers/keyboard.hpp"
#include "../memory/memory.hpp"
#include "../shared/types.hpp"
#include "../shell/shell.hpp"
#include "../shell/terminal.hpp"
#include "../storage/file_system.hpp"
#include "crti.hpp"
#include "timer.hpp"

inline void print_hex_byte(u8 b) {
  const char *hex = "0123456789ABCDEF";
  Terminal::putchar(hex[(b >> 4) & 0xF]);
  Terminal::putchar(hex[b & 0xF]);
}

inline void print_hex_bytes(const u8 *buf, int count) {
  for (int i = 0; i < count; i++) {
    print_hex_byte(buf[i]);
    Terminal::putchar(' ');
  }
  Terminal::print("\n");
}

extern "C" void kernel_main() {
  init_heap();
  call_constructors();
  Timer::init();

  Terminal::print("Step 1: pre-disk\n");

  Disk disk;
  FileSystem fs(disk);

  Terminal::print("Step 2: pre-format\n");
  fs.format();
  Terminal::print("Step 3: post-format\n");

  String sample_text = "Hello ROCK OS LOVERS";
  fs.write_file((char *)"hello.txt", (u8 *)sample_text.c_str(),
                (u32)sample_text.length() + 1);
  Terminal::print("Step 4: post-write\n");

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
