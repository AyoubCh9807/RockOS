#pragma once

#include "../data/colors.hpp"
#include "notification.hpp"
#include "notification_icons.hpp"

class NotificationService {

  static constexpr int MAX_NOTIFICATIONS = 16;
  static constexpr int INVALID_SLOT = -1;

  static constexpr u32 DEFAULT_NOTIFICATION_TIMEOUT = 3000;
  static constexpr const u32 *DEFAULT_ICON = ROCK_OS_ICON_BATTERY_CHARGING;

private:
  Notification notifications[MAX_NOTIFICATIONS];

  int count = 0;
  u16 used_slots = 0;

public:
  NotificationService() = default;

  void draw() {
    for (const auto &notification : notifications) {
      if (notification.visible())
        notification.draw();
    }
  }

  void update() {
    for (auto i{0uz}; i < MAX_NOTIFICATIONS; i++) {
      if (!is_slot_occupied(i))
        continue;

      notifications[i].update();

      if (!notifications[i].expired())
        continue;

      notifications[i].hide();
      free_slot(i);
      count--;
    }
  }

  void occupy_slot(int slot) { used_slots |= (u16(1) << slot); }

  void free_slot(int slot) { used_slots &= ~(u16(1) << slot); }

  bool is_slot_occupied(int slot) const {
    return used_slots & (u16(1) << slot);
  }

  int get_first_free_slot() const {
    for (int i = 0; i < MAX_NOTIFICATIONS; i++) {
      if (!is_slot_occupied(i))
        return i;
    }

    return INVALID_SLOT;
  }

  void add_notification(const String &title, const String &description,
                        u32 timeout, const u32 *icon) {
    const int slot = get_first_free_slot();

    if (slot == INVALID_SLOT)
      return;

    if (timeout == 0)
      timeout = DEFAULT_NOTIFICATION_TIMEOUT;

    if (icon == nullptr)
      icon = DEFAULT_ICON;

    notifications[slot] = Notification(title, description, timeout, icon);

    occupy_slot(slot);
    count++;

    notifications[slot].show();
  }

  void add_notification(const String &title, const String &description,
                        u32 timeout) {
    add_notification(title, description, timeout, DEFAULT_ICON);
  }

  void add_notification(const String &title, const String &description,
                        u32 timeout, const u32 *icon, u32 background_color,
                        u32 title_color, u32 description_color,
                        u32 border_color = Colors::BLACK,
                        u32 border_thickness = 0) {
    const int slot = get_first_free_slot();

    if (slot == INVALID_SLOT)
      return;

    if (timeout == 0)
      timeout = DEFAULT_NOTIFICATION_TIMEOUT;

    if (icon == nullptr)
      icon = DEFAULT_ICON;

    Notification &notification = notifications[slot];

    notification = Notification(title, description, timeout, icon);

    notification.set_background_color(background_color);
    notification.set_title_color(title_color);
    notification.set_description_color(description_color);
    notification.set_border_color(border_color);
    notification.set_border_thickness(border_thickness);

    occupy_slot(slot);
    count++;

    notification.show();
  }

  void hide_all() {
    for (auto &notification : notifications) {
      notification.hide();
    }
  }

  void show_all() {
    for (auto i{0uz}; i < MAX_NOTIFICATIONS; i++) {
      if (is_slot_occupied(i) && !notifications[i].expired())
        notifications[i].show();
    }
  }

  void destroy_all() {
    for (auto i{0uz}; i < MAX_NOTIFICATIONS; i++) {
      notifications[i].hide();
      notifications[i].expire();
      free_slot(i);
    }

    count = 0;
  }

  u32 get_default_timeout() const { return DEFAULT_NOTIFICATION_TIMEOUT; }

  int get_count() const { return count; }

  int get_max_notifications() const { return MAX_NOTIFICATIONS; }
};
