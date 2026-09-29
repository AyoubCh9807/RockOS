#pragma once

#include "click_widget.hpp"
#include "clock_widget.hpp"
#include "cpu_widget.hpp"
#include "date_widget.hpp"
#include "memory_widget.hpp"
#include "system_info_widget.hpp"
#include "uptime_widget.hpp"

class WidgetRegistry {
private:
  static constexpr int MAX_WIDGETS = 64;

  IWidget *widgets[MAX_WIDGETS];

  ClockWidget clock;
  CpuWidget cpu;
  DateWidget date;
  MemoryWidget memory;
  SystemInfoWidget system_info;
  UptimeWidget uptime;
  ClickMeWidget clickme;

  int count = 0;

public:
  WidgetRegistry() = default;

  void register_widget(IWidget *widget) {
    if (!widget || count >= MAX_WIDGETS)
      return;

    widgets[count++] = widget;
  }

  void register_widgets() {
    register_widget(&clock);
    register_widget(&cpu);
    register_widget(&date);
    register_widget(&memory);
    register_widget(&system_info);
    register_widget(&uptime);
    register_widget(&clickme);
  }

  IWidget *get(int index) {
    if (index < 0 || index >= count)
      return nullptr;

    return widgets[index];
  }

  int get_count() const { return count; }

  void clear() {
    for (int i = 0; i < count; i++)
      widgets[i] = nullptr;

    count = 0;
  }

  bool remove(IWidget *widget) {
    if (!widget)
      return false;

    for (int i = 0; i < count; i++) {
      if (widgets[i] != widget)
        continue;

      for (int j = i; j < count - 1; j++)
        widgets[j] = widgets[j + 1];

      widgets[count - 1] = nullptr;
      count--;

      return true;
    }

    return false;
  }
};
