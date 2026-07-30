constexpr int KEYBOARD_BUFFER_SIZE = 1024;

typedef unsigned char u8;
typedef unsigned long long size_t; 

unsigned char inb(unsigned short port) {
  unsigned char result;
  __asm__ volatile("inb %1, %0" : "=a"(result) : "Nd"(port));
  return result;
}

u8 *heap_start   = (u8 *)0x00400000;
u8 *heap_end     = (u8 *)0x00800000;
u8 *heap_current = (u8 *)0x00400000;

void* kmalloc(size_t size) {
  if(heap_current + size > heap_end) {
    return 0;
    // Out of memory
  }
  u8* ptr = heap_current;
  heap_current += size;
  return (void*)ptr;
}

void* kfree(u8* ptr) {
  // Nothin for now
}

char scancode_to_ascii(unsigned char scancode) {
  static const char ascii_map[128] = {
      0,   27,  '1',  '2',  '3',  '4', '5', '6',  '7', '8', '9', '0',
      '-', '=', '\b', '\t', 'a',  'z', 'e', 'r',  't', 'y', 'u', 'i',
      'o', 'p', '[',  ']',  '\n', 0,   'q', 's',  'd', 'f', 'g', 'h',
      'j', 'k', 'l',  'm',  '\'', '`', 0,   '\\', 'w', 'x', 'c', 'v',
      'b', 'n', ';',  ',',  '.',  '/', 0,   '*',  0,   ' '};

  if (scancode < 128) {
    return ascii_map[scancode];
  }
  return 0;
}

void print_string(const char *str) {
  volatile char *video_memory = (volatile char *)0xB8000;
  int i = 0;
  int vram_index = 0;

  while (str[i] != '\0') {
    if (str[i] == '\n') {
      vram_index = ((vram_index + 160) / 160) * 160;
    } else {
      video_memory[vram_index] = str[i];
      video_memory[vram_index + 1] = 0x0F;
      vram_index += 2;
    }
    i++;
  }

  video_memory[vram_index] = ' ';
  video_memory[vram_index + 1] = 0x0F;
}

extern "C" void kernel_main() {

  // print_string("Welcome to Rock OS");

  constexpr int confirmation_port = 0x64;
  constexpr int char_port = 0x60;
  constexpr int release_hex = 0x80;
  constexpr int special_key_hex = 0xE0;
  constexpr int delete_hex = 0x53;
  constexpr int backspace_hex = 0x0E;
  constexpr int enter_hex = 0x1C;

  // print_string("HEY I CAN SEE U");

  char buffer[KEYBOARD_BUFFER_SIZE];
  int current_buffer_index = 0;

  while (1) {

    // /*
    if (inb(confirmation_port) & 1) {
      unsigned char scancode = inb(char_port);

      if (scancode & release_hex)
        continue;

      if (scancode == backspace_hex) {
        if (current_buffer_index > 0) {
          buffer[current_buffer_index - 1] = '\0';
          current_buffer_index--;
        }

        print_string(buffer);
        continue;
      }

      if (scancode == enter_hex) {
        if (current_buffer_index < 256 - 3) {
          buffer[current_buffer_index] = '\n';
          current_buffer_index++;
          buffer[current_buffer_index] = '\0';
        }
        print_string(buffer);
        continue;
      }

      if (scancode == special_key_hex) {
        int next_scancode = inb(char_port);
        if (next_scancode & release_hex)
          continue;

        if (next_scancode == delete_hex) {
          buffer[current_buffer_index - 1] = '\0';
          current_buffer_index--;
          print_string(buffer);
          continue;
        }
      }

      char c = scancode_to_ascii(scancode);
      if (c != 0) {
        if (current_buffer_index <= 256 - 1) {
          buffer[current_buffer_index] = c;
          current_buffer_index++;
          buffer[current_buffer_index] = '\0';
          print_string(buffer);
        }
      }
    }

    // * */
  }
}
