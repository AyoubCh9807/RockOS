#pragma once

#include "../../data/colors.hpp"
#include "../../utils/terminal_utils.hpp"
#include "../window.hpp"
#include "../window_app.hpp"

#include "../../shell/shell.hpp"

class TerminalApp : public IWindowApp {
private:
  Terminal &terminal;
  Shell &shell;

public:
  TerminalApp(Terminal &terminal, Shell &shell)
      : terminal(terminal), shell(shell) {}

  const char *name() const override { return "Terminal"; }

  void on_create(Window &win) override {
    terminal.terminal_utils.resize(win.get_client_width(),
                                   win.get_client_height());

    shell.print_prompt();
    terminal.terminal_utils.begin_input();

    draw(win);
  }

  void on_draw(Window &win) override {
    terminal.terminal_utils.resize(win.get_client_width(),
                                   win.get_client_height());

    draw(win);
  }

  void history_up() {
    TextBuffer &buffer = terminal.terminal_utils.get_buffer();

    char current[1024];
    buffer.get_input_text(current, sizeof(current));

    String current_buffer = current;

    if (shell.get_history().up(current_buffer))
      buffer.set_input(current_buffer.c_str());
  }

  void history_down() {
    TextBuffer &buffer = terminal.terminal_utils.get_buffer();

    char current[1024];
    buffer.get_input_text(current, sizeof(current));

    String current_buffer = current;

    if (shell.get_history().down(current_buffer))
      buffer.set_input(current_buffer.c_str());
  }

  void on_key(Window &win, const KeyEvent &ev) override {
    TerminalUtils &utils = terminal.terminal_utils;

    switch (ev.keytype) {
    case KeyType::Char:
      utils.insert_char(ev.scancode);
      break;

    case KeyType::BackSpace:
      utils.backspace();
      break;

    case KeyType::ArrowLeft:
      utils.move_left();
      break;

    case KeyType::ArrowRight:
      utils.move_right();
      break;

    case KeyType::ArrowUp:
      history_up();
      break;

    case KeyType::ArrowDown:
      history_down();
      break;
    case KeyType::Enter:
      execute_command();
      break;

    default:
      return;
    }

    draw(win);
  }

  void on_mouse_event(Window &win, const MouseEvent &) override {}

private:
  void execute_command() {
    TextBuffer &buffer = terminal.terminal_utils.get_buffer();

    char cmd[1024];
    int length = buffer.get_input_text(cmd, sizeof(cmd));

    terminal.terminal_utils.putchar('\n');

    if (length > 0) {
      CommandResult res = shell.execute(cmd);
      terminal.terminal_utils.print(res.output.c_str(), res.color);
    }

    terminal.terminal_utils.print("\n");

    shell.print_prompt();
    buffer.begin_input();
  }

  void draw(Window &win) { terminal.terminal_utils.render(win); }
};
;
