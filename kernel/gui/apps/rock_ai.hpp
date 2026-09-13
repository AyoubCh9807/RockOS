#pragma once

#include "../../ai/rock_ai.hpp"

#include "../window.hpp"
#include "../window_app.hpp"

#include "../../containers/string.hpp"
#include "../../containers/vector.hpp"

class RockAIApp : public IWindowApp {
private:
  static constexpr int MAX_INPUT_LENGTH = 128;
  static constexpr int MAX_MESSAGES = 32;

  struct Message {
    bool from_user;
    String text;
  };

  Vocabulary vocab;
  RockAI ai;

  Vector<Message> messages;

  char input[MAX_INPUT_LENGTH];
  int input_length = 0;

  int scroll = 0;

private:
  void add_message(bool from_user, const char *text) {

    if (messages.size() >= MAX_MESSAGES)
      messages.pop_front();

    Message message;
    message.from_user = from_user;
    message.text = String(text);

    messages.push_back(message);

    scroll = 0;
  }

  void submit_message() {

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
  RockAIApp() : ai(vocab) { input[0] = '\0'; }

  const char *name() const override { return "Rock AI"; }

  void on_create(Window &win) override {

    if (ai.is_ready()) {

      add_message(false, "Yo! Rock AI is online. 🤘");

    } else {

      add_message(false, "Rock AI failed to load.");
    }

    draw(win);
  }

  void draw(Window &win) {

    win.clear(Colors::DARK_RED);

    win.draw_string("ROCK AI", 20, 20, Colors::WHITE);

    win.draw_string(ai.is_ready() ? "ONLINE" : "OFFLINE", 500, 20,
                    ai.is_ready() ? Colors::GOLD : Colors::RED);

    int y = 60;

    for (int i = scroll; i < messages.size(); i++) {

      if (y > 360)
        break;

      const char *prefix = messages[i].from_user ? "You: " : "Rock AI: ";

      u32 prefix_color = messages[i].from_user ? Colors::GOLD : Colors::WHITE;

      win.draw_string(prefix, 20, y, prefix_color);

      win.draw_string(messages[i].text.c_str(), 80, y, Colors::WHITE);

      y += 32;
    }

    win.draw_string(">", 20, 420, Colors::GOLD);

    win.draw_string(input, 40, 420, Colors::WHITE);
  }

  void on_draw(Window &win) override { draw(win); }

  void on_key(Window &win, const KeyEvent &ev) override {

    if (ev.keytype == KeyType::Enter) {

      submit_message();

      draw(win);

      return;
    }

    if (ev.keytype == KeyType::BackSpace) {

      if (input_length > 0) {

        input_length--;

        input[input_length] = '\0';
      }

      draw(win);

      return;
    }

    /*
     * Add printable-key handling here.
     * Characters can be appended directly
     * to input using input_length.
     */

    if (ev.keytype == KeyType::Char) {
      if (input_length < MAX_INPUT_LENGTH) {
        input[input_length] = ev.scancode;
        input_length++;
      }
      draw(win);
      return;
    }
  }

  void on_mouse_event(Window &win, const MouseEvent &ev) override {

    if (ev.button_type != MouseButton::LEFT_BUTTON ||
        ev.event_type != MouseEventType::PRESS)
      return;

    draw(win);
  }
};
