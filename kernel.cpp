void print_string(const char *str) {
  volatile char *video_memory = (volatile char *)0xB8000;
  int i = 0;
  while (str[i] != '\0') {
    video_memory[i * 2] = str[i];
    video_memory[i * 2 + 1] = 0x0F;
    i++;
  }
}

extern "C" void kernel_main() {

  print_string("Welcome to Damian OS");

  while (1) {
    // Infinite loop to keep the kernel running safely
  }
}
