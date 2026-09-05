#pragma once

#include "dialog.hpp"

constexpr int MAX_DIALOGS = 16;

class DialogManager {
private:
  Dialog *dialogs[MAX_DIALOGS]{};
  int count = 0;

  Dialog *active_dialog = nullptr;

public:
  DialogManager() = default;

  Dialog *create_dialog(int x, int y, int width, int height, const char *title,
                        const char *message) {
    if (count >= MAX_DIALOGS)
      return nullptr;

    Dialog *dialog = new Dialog(x, y, width, height, title, message);

    dialogs[count++] = dialog;

    return dialog;
  }

  void show(Dialog *dialog) {
    if (!dialog)
      return;

    active_dialog = dialog;
    dialog->show();
  }

  void close() {
    if (!active_dialog)
      return;

    active_dialog->close();
    active_dialog = nullptr;
  }

  Dialog *get_active() const { return active_dialog; }

  bool has_active() const { return active_dialog != nullptr; }

  void render() {
    if (active_dialog)
      active_dialog->draw();
  }

  void destroy_dialog(Dialog *dialog) {
    if (!dialog)
      return;

    if (active_dialog == dialog)
      active_dialog = nullptr;

    for (int i = 0; i < count; i++) {
      if (dialogs[i] != dialog)
        continue;

      delete dialog;

      for (int j = i; j < count - 1; j++)
        dialogs[j] = dialogs[j + 1];

      dialogs[count - 1] = nullptr;
      count--;

      return;
    }
  }

  int get_count() const { return count; }

  void route_key(KeyEvent &ev) {
    if (!active_dialog)
      return;

    if (!active_dialog->visible)
      return;

    bool handled = active_dialog->handle_key(ev);

    if (handled && !active_dialog->visible)
      active_dialog = nullptr;
  }
};
