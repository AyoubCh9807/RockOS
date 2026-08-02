#include "kernel.hpp"
#include "crti.hpp"
#include "../memory/memory.hpp"
#include "../shell/shell.hpp"
#include "../shell/terminal.hpp"
#include "timer.hpp"
#include "../drivers/keyboard.hpp"

extern "C" void kernel_main() {

  init_heap();
  call_constructors();

  Timer::init();

  Shell shell;
  shell.run();

  while (1) {
    Kernel::halt();
  }
}
