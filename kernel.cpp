#include "kernel.hpp"

extern "C" void kernel_main() {
  char buffer[KEYBOARD_BUFFER_SIZE];
  int current_buffer_index = 0;
  buffer[0] = '\0';

  Terminal::print("Rock OS Shell\n> ");

  while (1) {
    if (inb(confirmation_port) & 1) {
      unsigned char scancode = inb(char_port);

      if (scancode & release_hex)
        continue;

      if (scancode == backspace_hex) {
        if (current_buffer_index > 0) {
          current_buffer_index--;
          buffer[current_buffer_index] = '\0';
          Terminal::putchar('\b');
        }
        continue;
      }

      if (scancode == enter_hex) {
        buffer[current_buffer_index] = '\0';
        Terminal::putchar('\n');

        // Command parsing logic goes here (e.g., check if buffer == "help")
        const char* args[10];
        int max_args = 10; 
        Terminal::print(Terminal::parse(buffer, args, max_args));

        current_buffer_index = 0;
        buffer[0] = '\0';
        continue;
      }

      if (scancode == special_key_hex) {
        int next_scancode = inb(char_port);
        if (next_scancode & release_hex)
          continue;

        if (next_scancode == delete_hex && current_buffer_index > 0) {
          current_buffer_index--;
          buffer[current_buffer_index] = '\0';
          Terminal::putchar('\b');
          continue;
        }
      }

      char c = scancode_to_ascii(scancode);
      if (c != 0) {
        if (current_buffer_index < KEYBOARD_BUFFER_SIZE - 1) {
          buffer[current_buffer_index] = c;
          current_buffer_index++;
          buffer[current_buffer_index] = '\0';
          Terminal::putchar(c);
        }
      }
    }
  }
}
