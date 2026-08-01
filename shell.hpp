#pragma once

#include "containers/string.hpp"
#include "keyboard.hpp"
#include "terminal.hpp"

class Shell {
private:
  Keyboard keyboard;
  String buffer;

public:
  Shell() : keyboard(), buffer("") {};

  void run() {
    Terminal::print("Rock OS Shell\n> ");

    while (1) {
      char c = keyboard.poll();
      if (c == 0) continue; // Ignore empty polls

      // 1. Handle Backspace
      if (c == '\b') {
        if (buffer.length() > 0) {
          // Remove last char from memory buffer. pop_back() decrements
          // the String's internal size (unlike writing '\0' through
          // operator[], which left length() stale).
          buffer.pop_back();
          // Tell terminal to erase it visually
          Terminal::putchar('\b');
        }
      } 
      // 2. Handle Enter / Newline
      else if (c == '\n') {
        Terminal::putchar('\n');

        const char* args[10];
        int max_args = 10;

        // Make a mutable copy of the buffer so split_by can safely modify it
        char cmd_copy[256];
        int i = 0;
        while (buffer.c_str()[i] != '\0' && i < 255) {
            cmd_copy[i] = buffer.c_str()[i];
            i++;
        }
        cmd_copy[i] = '\0';

        // Parse and print output
        Terminal::print(Terminal::parse(cmd_copy, args, max_args));

        // Reset buffer and print the next prompt
        buffer = String("");
        Terminal::print("> ");
      } 
      // 3. Handle Normal Typing Characters
      else {
        buffer = buffer + c;
        Terminal::putchar(c);
      }
    }
  }
};
