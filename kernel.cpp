#include "kernel.hpp"
#include "crti.hpp"
#include "memory.hpp"
#include "shell.hpp"
#include "terminal.hpp"
#include "timer.hpp"

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
