#pragma once

#include "../containers/string.hpp"

class ShellHistory {
private:
  static constexpr int MAX_HISTORY = 32;

  String history[MAX_HISTORY];

  int total_history_items = 0;
  int history_wrapper_index = 0;

  // Position used while navigating history.
  // total_history_items means "the draft/current command".
  int selected_index = 0;

  String draft;

public:
  ShellHistory() {
    for (int i = 0; i < MAX_HISTORY; i++)
      history[i] = "";
  }

  void add(const String &command) {
    if (command.length() <= 0)
      return;

    history[history_wrapper_index] = command;

    history_wrapper_index = (history_wrapper_index + 1) % MAX_HISTORY;

    if (total_history_items < MAX_HISTORY)
      total_history_items++;

    // Return to the position after the newest command.
    selected_index = total_history_items;
  }

  void reset_navigation() {
    selected_index = total_history_items;
    draft = "";
  }

  void start_navigation(const String &current_buffer) {
    if (selected_index == total_history_items)
      draft = current_buffer;
  }

  bool up(String &buffer) {
    if (total_history_items == 0)
      return false;

    if (selected_index == total_history_items)
      draft = buffer;

    if (selected_index <= 0)
      return false;

    selected_index--;

    buffer = get(selected_index);
    return true;
  }

  bool down(String &buffer) {
    if (selected_index >= total_history_items)
      return false;

    selected_index++;

    if (selected_index == total_history_items)
      buffer = draft;
    else
      buffer = get(selected_index);

    return true;
  }

  int size() const { return total_history_items; }

  String get(int index) const {
    // If the buffer hasn't wrapped, history is already
    // stored chronologically from index 0.
    if (total_history_items < MAX_HISTORY)
      return history[index];

    // Once full, history_wrapper_index points to the oldest
    // entry.
    int physical_index = (history_wrapper_index + index) % MAX_HISTORY;

    return history[physical_index];
  }
};
