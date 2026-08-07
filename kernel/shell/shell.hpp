#pragma once

#include "../containers/string.hpp"
#include "../core/kernel.hpp"
#include "../drivers/keyboard.hpp"
#include "../shared/key_event.hpp"
#include "terminal.hpp"

constexpr static int SHELL_MAX_AGRGS = 16;

class Shell {
private:
  Terminal &terminal;

  // What the user is currently typing
  String buffer;
  static constexpr int MAX_HISTORY = 32;

  // Keep command history
  String history[MAX_HISTORY];

  // Keep track of how many history items we added
  int total_history_items = 0;

  // Keep track of a wrapper index
  int history_wrapper_index = 0;

  // Keep track of what the user is actually typing
  String draft;

  // Index into `history` currently being shown while browsing with the
  // arrow keys. Ranges from 0 (oldest) to total_history_items (meaning
  // "not browsing / blank line", one past the newest entry).
  int currently_selected_history_item = 0;

public:
  Shell(Terminal &t) : terminal(t), buffer("") {
    for (int i = 0; i < MAX_HISTORY; i++) {
      history[i] = "";
    }
  };

  void add_to_history(String &text) {
    if (text.length() <= 0)
      return;
    if (history_wrapper_index >= 0 && history_wrapper_index < MAX_HISTORY) {
      history[history_wrapper_index] = text;
      history_wrapper_index = (history_wrapper_index + 1) % MAX_HISTORY;
      if (total_history_items < MAX_HISTORY)
        total_history_items++;
    }
    // Reset the browse cursor so the next Up press starts from the most
    // recently entered command, instead of tracking total_history_items
    // in lockstep (which made currently_selected_history_item ==
    // total_history_items right after every add and permanently failed
    // the "< total_history_items" guard in the arrow-key handlers below).
    currently_selected_history_item = total_history_items;
  }

  void run() {
    TerminalUtils::print("Rock OS Shell\n> ");

    while (1) {
      KeyEvent ev = Keyboard::read();
      if (ev.scancode == 0 || ev.keytype == KeyType::None) {
        Kernel::halt();
        continue;
      }; // wait safely

      // Handle Backspace
      if (ev.keytype == KeyType::BackSpace && ev.scancode == '\b') {
        if (buffer.length() > 0) {
          // Remove last char from memory buffer. pop_back() decrements
          // the String's internal size (unlike writing '\0' through
          // operator[], which left length() stale).
          buffer.pop_back();
          // Tell terminal to erase it visually
          TerminalUtils::putchar('\b');
        }
      }
      // Handle Enter / Newline
      else if (ev.keytype == KeyType::Enter && ev.scancode == '\n') {

        add_to_history(buffer);

        TerminalUtils::putchar('\n');

        char *args[SHELL_MAX_AGRGS];
        int max_args = SHELL_MAX_AGRGS;

        // Make a mutable copy of the buffer so split_by can safely modify it
        char cmd_copy[256];
        int i = 0;
        while (buffer.c_str()[i] != '\0' && i < 255) {
          cmd_copy[i] = buffer.c_str()[i];
          i++;
        }
        cmd_copy[i] = '\0';

        // Parse and print output
        TerminalUtils::print(terminal.parse(cmd_copy, args, max_args));

        // Reset buffer and print the next prompt
        buffer = String("");
        TerminalUtils::print("\n> ");
      }
      // Handle Arrow Up: walk back to older history entries
      else if (ev.keytype == KeyType::ArrowUp) {

        if (currently_selected_history_item == total_history_items) {
          draft = buffer;
        }

        if (currently_selected_history_item > 0) {
          currently_selected_history_item--;

          // Erase what's currently on screen BEFORE swapping the buffer
          // content, so we erase the right number of characters.
          for (int j = 0; j < buffer.length(); j++) {
            TerminalUtils::putchar('\b');
          }

          buffer = history[currently_selected_history_item];
          TerminalUtils::print(buffer.c_str());
        }
      }
      // Handle Arrow Down: walk forward to newer history entries, or
      // back to a blank line once past the newest entry.
      else if (ev.keytype == KeyType::ArrowDown) {

        if (currently_selected_history_item < total_history_items) {
          currently_selected_history_item++;

          for (int j = 0; j < buffer.length(); j++) {
            TerminalUtils::putchar('\b');
          }

          if (currently_selected_history_item == total_history_items) {
            buffer = draft;
          } else {
            buffer = history[currently_selected_history_item];
          }
          TerminalUtils::print(buffer.c_str());
        }
      }
      // Cursor movement isn't implemented yet - swallow these instead of
      // letting them fall into the "normal typing" branch below, which
      // would otherwise insert raw scancodes 128/129 into the buffer.
      else if (ev.keytype == KeyType::ArrowLeft ||
               ev.keytype == KeyType::ArrowRight) {
        // no-op for now
      }
      // Handle Normal Typing Characters
      else {
        // Any manual edit exits history browsing, so the next Up press
        // starts fresh from the most recent command again.
        currently_selected_history_item = total_history_items;
        buffer = buffer + ev.scancode;
        draft = buffer;
        TerminalUtils::putchar(ev.scancode);
      }
    }
  }
};
