#pragma once

#include "../../../boot/graphics.hpp"
#include "../../ai/rock_ai.hpp"

#include "../window.hpp"
#include "../window_app.hpp"

class RockAIApp : public IWindowApp {
private:
  static constexpr int MAX_INPUT = 128;
  static constexpr int MAX_MESSAGES = 24;
  static constexpr int INPUT_LENGTH = 256;

  static constexpr int CHAR_W = Graphics::CHARACTER_WIDTH;
  static constexpr int CHAR_H = Graphics::CHARACTER_HEIGHT;

  static constexpr int HEADER_HEIGHT = 32;
  static constexpr int INPUT_HEIGHT = 48;
  static constexpr int INPUT_Y = 400;
  ;
  static constexpr int INPUT_MARGIN = 8;
  static constexpr int MESSAGE_GAP = 8;
  static constexpr int MESSAGE_PADDING = 8;

  static constexpr u32 BG = 0x0B0B0B;
  static constexpr u32 HEADER_BG = 0x1A0505;

  static constexpr u32 USER_BG = 0x7A0C0C;
  static constexpr u32 AI_BG = 0x161616;

  static constexpr u32 USER_BORDER = 0xFFD700;
  static constexpr u32 AI_BORDER = 0xB22222;

  static constexpr u32 TEXT = 0xF2F2F2;
  static constexpr u32 ACCENT = 0xFFD700;
  static constexpr u32 MUTED = 0x555555;

  static constexpr u32 ONLINE = 0x2ECC40;
  static constexpr u32 OFFLINE = 0xFF3B3B;

  struct Message {
    char text[INPUT_LENGTH];
    bool from_user;
  };

  Vocabulary vocab;
  RockAI ai;

  Message messages[MAX_MESSAGES];
  int message_count = 0;

  char input[MAX_INPUT];
  int input_length = 0;

  int scroll = 0;
  int cursor_tick = 0;

private:
  static int text_width(const char *text) {
    int length = 0;

    while (text[length] != '\0')
      length++;

    return length * CHAR_W;
  }

  static void copy_text(char *destination, const char *source, int max_length) {
    int i = 0;

    while (i < max_length - 1 && source[i] != '\0') {
      destination[i] = source[i];
      i++;
    }

    destination[i] = '\0';
  }

  static void border_rect(Window &win, int x, int y, int width, int height,
                          u32 fill, u32 border) {
    win.draw_rect(x, y, width, height, fill);

    // Draw the four edges using 1-pixel rectangles. Window does not
    // need a separate line primitive for simple rectangular borders.
    win.draw_rect(x, y, width, 1, border);
    win.draw_rect(x, y + height - 1, width, 1, border);

    win.draw_rect(x, y, 1, height, border);
    win.draw_rect(x + width - 1, y, 1, height, border);
  }

  void add_message(bool from_user, const char *text) {
    // Keep the conversation bounded so the app does not grow
    // indefinitely as the user keeps chatting.
    if (message_count == MAX_MESSAGES) {
      for (int i = 1; i < MAX_MESSAGES; i++)
        messages[i - 1] = messages[i];

      message_count--;
    }

    Message &message = messages[message_count++];

    copy_text(message.text, text, INPUT_LENGTH);
    message.from_user = from_user;

    scroll = 0;
  }

  int wrap_text(const char *text, char lines[][INPUT_LENGTH], int max_lines,
                int max_chars) const {
    if (max_lines <= 0 || max_chars <= 0)
      return 0;

    int line = 0;
    int i = 0;

    while (text[i] != '\0' && line < max_lines) {
      while (text[i] == ' ')
        i++;

      if (text[i] == '\0')
        break;

      const int start = i;

      int length = 0;
      int last_space = -1;

      // Keep words together where possible. If the line becomes full,
      // use the last space so the next line starts at a whole word.
      while (text[i] != '\0' && length < max_chars) {
        if (text[i] == ' ')
          last_space = i;

        i++;
        length++;
      }

      if (text[i] != '\0' && length == max_chars && last_space >= start) {
        i = last_space + 1;
        length = last_space - start;
      }

      for (int j = 0; j < length; j++)
        lines[line][j] = text[start + j];

      lines[line][length] = '\0';
      line++;
    }

    return line;
  }

  void draw_header(Window &win) {
    win.draw_rect(0, 0, win.width, HEADER_HEIGHT, HEADER_BG);

    win.draw_rect(0, HEADER_HEIGHT - 1, win.width, 1, Colors::RED);

    win.draw_string("ROCK AI", 12, 12, ACCENT);

    const bool ready = ai.is_ready();
    const u32 status_color = ready ? ONLINE : OFFLINE;

    win.draw_rect(win.width - 72, 12, 6, 6, status_color);

    win.draw_string(ready ? "ON" : "OFF", win.width - 58, 11, status_color);
  }

  void draw_message(const Message &message, Window &win, int &y) {
    const int content_width = win.width - 40;

    const int max_chars = (content_width - MESSAGE_PADDING * 2) / CHAR_W;

    char lines[16][INPUT_LENGTH];

    const int line_count = wrap_text(message.text, lines, 16, max_chars);

    if (line_count == 0)
      return;

    int widest = 0;

    for (int i = 0; i < line_count; i++) {
      const int width = text_width(lines[i]);

      if (width > widest)
        widest = width;
    }

    int bubble_width = widest + MESSAGE_PADDING * 2;

    if (bubble_width < 60)
      bubble_width = 60;

    if (bubble_width > content_width)
      bubble_width = content_width;

    const int bubble_height = line_count * (CHAR_H + 4) + MESSAGE_PADDING * 2;

    const int x = message.from_user ? win.width - bubble_width - 20 : 20;

    const u32 fill = message.from_user ? USER_BG : AI_BG;

    const u32 border = message.from_user ? USER_BORDER : AI_BORDER;

    border_rect(win, x, y, bubble_width, bubble_height, fill, border);

    int text_y = y + MESSAGE_PADDING;

    for (int i = 0; i < line_count; i++) {
      win.draw_string(lines[i], x + MESSAGE_PADDING, text_y, TEXT);

      text_y += CHAR_H + 4;
    }

    y += bubble_height + MESSAGE_GAP;
  }

  void draw_messages(Window &win) {
    int y = HEADER_HEIGHT + 10;
    const int bottom = INPUT_Y - 10;

    for (int i = scroll; i < message_count; i++) {
      const int previous_y = y;

      draw_message(messages[i], win, y);

      if (previous_y >= bottom || y > bottom)
        break;
    }
  }

  void draw_input(Window &win) {
    const int y = INPUT_Y;

    border_rect(win, 10, y, win.width - 20, INPUT_HEIGHT, 0x141414,
                Colors::DARK_RED);

    win.draw_string(">", 20, y + 16, ACCENT);

    if (input_length == 0) {
      win.draw_string("Type a message...", 36, y + 16, MUTED);
    } else {
      const int visible_chars = (win.width - 52) / CHAR_W;

      int start = 0;

      if (input_length > visible_chars)
        start = input_length - visible_chars;

      win.draw_string(input + start, 36, y + 16, TEXT);

      if ((cursor_tick / 15) % 2 == 0) {
        const int shown = input_length - start;

        win.draw_rect(36 + shown * CHAR_W, y + 14, 1, CHAR_H + 2, ACCENT);
      }
    }
  }

  void submit(Window &win) {
    if (input_length == 0)
      return;

    input[input_length] = '\0';

    add_message(true, input);

    ai.send_request(input);

    const AIResponse &response = ai.get_response();

    add_message(false, response.text().c_str());

    input_length = 0;
    input[0] = '\0';
  }

public:
  RockAIApp() : vocab(), ai(vocab) { input[0] = '\0'; }

  const char *name() const override { return "Rock AI"; }

  void on_create(Window &win) override {
    if (ai.is_ready())
      add_message(false, "Yo! Rock AI is online.");
    else
      add_message(false, "Rock AI failed to load.");

    draw(win);
  }

  void draw(Window &win) {
    win.clear(BG);

    draw_header(win);
    draw_messages(win);
    draw_input(win);
  }

  void on_draw(Window &win) override {
    cursor_tick++;
    draw(win);
  }

  void on_key(Window &win, const KeyEvent &event) override {
    if (event.keytype == KeyType::BackSpace) {
      if (input_length > 0) {
        input_length--;
        input[input_length] = '\0';
      }
      return;
    }

    if (event.keytype == KeyType::Enter) {
      submit(win);
      return;
    }

    if (event.keytype == KeyType::Char) {
      if (input_length < INPUT_LENGTH - 1) {
        input[input_length++] = event.scancode;
        input[input_length] = '\0';
      }
    }
  }

  void on_mouse_event(Window &win, const MouseEvent &ev) override {
    if (ev.button_type != MouseButton::LEFT_BUTTON)
      return;

    if (ev.event_type != MouseEventType::PRESS)
      return;

    draw(win);
  }
};
