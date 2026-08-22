#pragma once

#include "../containers/string.hpp"
#include "../core/kernel.hpp"
#include "../data/user_info.hpp"
#include "../drivers/keyboard.hpp"
#include "../shared/key_event.hpp"
#include "../utils/string_utils.hpp"
#include "../utils/terminal_utils.hpp"
#include "environment.hpp"
#include "shell_history.hpp"
#include "terminal.hpp"

constexpr static int SHELL_MAX_ARGS = 64;

class Shell {
private:
  Terminal &terminal;
  Environment &env;
  ShellHistory &history;

  String buffer;

  int command_start;
  int command_end;

  void dump_root_sanity(FileSystem &fs) {
    Inode root{};
    terminal.fs.get_inode_manager().read_inode(ROOT_INODE, root);
    Debugger::log("ROOT CHECK: used=");
    Debugger::log_number(root.used);
    Debugger::log(" dir=");
    Debugger::log_number(root.is_directory);
    Debugger::log(" block0=");
    Debugger::log_number(root.direct_blocks[0]);
    Debugger::log("\n");

    u8 buf[BLOCK_SIZE];
    terminal.fs.get_block_manager().read_block(root.direct_blocks[0], buf);
    DirectoryEntry *entries = (DirectoryEntry *)buf;
    for (int i = 0; i < DIRECTORY_ENTRIES_PER_BLOCK; i++) {
      if (entries[i].is_used) {
        Debugger::log("  entry: ");
        Debugger::log(entries[i].name);
        Debugger::log(" inode=");
        Debugger::log_number(entries[i].inode_number);
        Debugger::log("\n");
      }
    }
  }

  const int get_cursor() const {
    return terminal.terminal_utils.get_cursor_position();
  }

  inline void print_formatted(unsigned color, const char *fmt, ...) {
    char buf[256];
    va_list args;
    va_start(args, fmt);
    StringUtils::vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    terminal.terminal_utils.print(buf, color);
  }

  void print_prompt() {
    terminal.terminal_utils.print_formatted(0xFFFFFF, "\n[%s@%s %s]$ ", USER,
                                            OS, terminal.get_current_path()),

        command_start = get_cursor();
    command_end = command_start;
  }
  void execute_command() {
    history.add(buffer);

    terminal.terminal_utils.putchar('\n');

    char *args[SHELL_MAX_ARGS];
    int max_args = SHELL_MAX_ARGS;

    char cmd_copy[256];

    int i = 0;
    while (buffer.c_str()[i] != '\0' && i < 255) {
      cmd_copy[i] = buffer.c_str()[i];
      i++;
    }

    cmd_copy[i] = '\0';

    CommandResult res = terminal.parse(cmd_copy, args, max_args);

    terminal.terminal_utils.print(res.output.c_str(), res.color);

    buffer = String("");

    print_prompt();
    dump_root_sanity(terminal.fs);
  }

  void handle_cursor(const KeyEvent &ev) {
    if (ev.keytype == KeyType::ArrowLeft) {
      if (get_cursor() > command_start)
        terminal.terminal_utils.move_left();
    }

    else if (ev.keytype == KeyType::ArrowRight) {
      if (get_cursor() < command_end)
        terminal.terminal_utils.move_right();
    }
  }

  void handle_backspace() {
    if (buffer.length() <= 0)
      return;

    int cursor = get_cursor();

    if (cursor <= command_start)
      return;

    int buffer_index = cursor - command_start - 1;

    buffer.remove(buffer_index);

    terminal.terminal_utils.set_input_end(command_end);
    terminal.terminal_utils.putchar('\b');

    command_end--;
  }

  void handle_history_up() {
    if (!history.up(buffer))
      return;

    int old_length = command_end - command_start;

    // Erase the currently displayed command.
    for (int i = 0; i < old_length; i++)
      terminal.terminal_utils.backspace();

    // Put the new command on screen.
    terminal.terminal_utils.print(buffer.c_str(), 0xFFFFFF);

    command_end = command_start + buffer.length();
    terminal.terminal_utils.set_input_end(command_end);
  }

  void handle_history_down() {
    if (!history.down(buffer))
      return;

    int old_length = command_end - command_start;

    // Erase the currently displayed command.
    for (int i = 0; i < old_length; i++)
      terminal.terminal_utils.backspace();

    // Put the new command on screen.
    terminal.terminal_utils.print(buffer.c_str(), 0xFFFFFF);

    command_end = command_start + buffer.length();
    terminal.terminal_utils.set_input_end(command_end);
  }

  void handle_character(char c) {
    int buffer_index = get_cursor() - command_start;
    buffer.insert(buffer_index, c);
    history.reset_navigation();

    terminal.terminal_utils.insert_char(TerminalUtils::Cell(c, 0xFFFFFF),
                                        command_end);
    command_end++;
  }

  void handle_key(const KeyEvent &ev) {
    if (ev.keytype == KeyType::BackSpace && ev.scancode == '\b') {
      handle_backspace();
    }

    else if (ev.keytype == KeyType::Enter && ev.scancode == '\n') {
      execute_command();
    }

    else if (ev.keytype == KeyType::ArrowUp) {
      handle_history_up();
    }

    else if (ev.keytype == KeyType::ArrowDown) {
      handle_history_down();
    }

    else if (ev.keytype == KeyType::ArrowLeft ||
             ev.keytype == KeyType::ArrowRight) {
      handle_cursor(ev);
    }

    else {
      handle_character(ev.scancode);
    }
  }

public:
  Shell(Terminal &t, ShellHistory &sh)
      : terminal(t), env(t.get_env()), history(sh), buffer("") {}

  void run() {

    bool is_status_bar_updated = false;

    terminal.draw_random_ascii();

    print_prompt();

    while (1) {

      if (heap.get_event_stream().is_event_pending()) {
        while (heap.get_event_stream().is_event_pending()) {
          heap.get_event_stream().consume_next_event();
        }
        is_status_bar_updated = false;
      }
      if (Timer::bar_dirty) {
        Timer::bar_dirty = false;
        TerminalUtils::update_status_bar();
      }

      KeyEvent ev = Keyboard::read();

      if (ev.scancode == 0 || ev.keytype == KeyType::None) {
        Kernel::halt();
        continue;
      }

      handle_key(ev);
    }
  }
};
