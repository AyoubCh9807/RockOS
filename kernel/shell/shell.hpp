#pragma once

#include "../containers/string.hpp"
#include "../core/kernel.hpp"
#include "../drivers/keyboard.hpp"
#include "../shared/key_event.hpp"
#include "../utils/fs_utils.hpp"
#include "../utils/string_utils.hpp"
#include "../utils/terminal_utils.hpp"
#include "terminal.hpp"

constexpr static int SHELL_MAX_AGRGS = 16;

class Shell {
private:
  Terminal &terminal;

  String buffer;

  static constexpr int MAX_HISTORY = 32;
  String history[MAX_HISTORY];

  int total_history_items = 0;
  int history_wrapper_index = 0;

  String draft;
  int currently_selected_history_item = 0;

public:
  Shell(Terminal &t) : terminal(t), buffer("") {
    for (int i = 0; i < MAX_HISTORY; i++)
      history[i] = "";
  }

  void add_to_history(String &text) {
    if (text.length() <= 0)
      return;

    if (history_wrapper_index >= 0 && history_wrapper_index < MAX_HISTORY) {
      history[history_wrapper_index] = text;
      history_wrapper_index = (history_wrapper_index + 1) % MAX_HISTORY;
      if (total_history_items < MAX_HISTORY)
        total_history_items++;
    }

    currently_selected_history_item = total_history_items;
  }

  void run() {
    
    TerminalUtils::print("\nRock OS Shell\n> ");

    while (1) {
      KeyEvent ev = Keyboard::read();
      if (ev.scancode == 0 || ev.keytype == KeyType::None) {
        Kernel::halt();
        continue;
      }

      if (ev.keytype == KeyType::BackSpace && ev.scancode == '\b') {
        if (buffer.length() > 0) {
          buffer.pop_back();
          TerminalUtils::putchar('\b');
        }
      } else if (ev.keytype == KeyType::Enter && ev.scancode == '\n') {
        add_to_history(buffer);
        TerminalUtils::putchar('\n');

        char *args[SHELL_MAX_AGRGS];
        int max_args = SHELL_MAX_AGRGS;

        char cmd_copy[256];
        int i = 0;
        while (buffer.c_str()[i] != '\0' && i < 255) {
          cmd_copy[i] = buffer.c_str()[i];
          i++;
        }
        cmd_copy[i] = '\0';

        TerminalUtils::print(terminal.parse(cmd_copy, args, max_args).c_str());

        buffer = String("");
        constexpr const char* USER = "Ayoubch";
        constexpr const char* OS = "rockos";
        TerminalUtils::print(StringUtils::format("\n[%s@%s %s]$ ", USER, OS, terminal.get_current_path()));

      } else if (ev.keytype == KeyType::ArrowUp) {
        if (currently_selected_history_item == total_history_items) {
          draft = buffer;
        }

        if (currently_selected_history_item > 0) {
          currently_selected_history_item--;

          for (int j = 0; j < buffer.length(); j++) {
            TerminalUtils::putchar('\b');
          }

          buffer = history[currently_selected_history_item];
          TerminalUtils::print(buffer.c_str());
        }
      } else if (ev.keytype == KeyType::ArrowDown) {
        if (currently_selected_history_item < total_history_items) {
          currently_selected_history_item++;

          for (int j = 0; j < buffer.length(); j++) {
            TerminalUtils::putchar('\b');
          }

          if (currently_selected_history_item == total_history_items)
            buffer = draft;
          else
            buffer = history[currently_selected_history_item];

          TerminalUtils::print(buffer.c_str());
        }
      } else if (ev.keytype == KeyType::ArrowLeft ||
                 ev.keytype == KeyType::ArrowRight) {
        // no-op
      } else {
        currently_selected_history_item = total_history_items;
        buffer = buffer + ev.scancode;
        draft = buffer;
        TerminalUtils::putchar(ev.scancode);
      }
    }
  }
};
