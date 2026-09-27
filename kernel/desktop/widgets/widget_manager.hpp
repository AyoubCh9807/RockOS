#pragma once

#include "../../shared/mouse_types.hpp"
#include "iwidget.hpp"
#include "widget_registry.hpp"

class WidgetManager {
private:
  WidgetRegistry &widget_registry;

public:
  WidgetManager(WidgetRegistry &wr)
      : widget_registry(wr) {}

  WidgetRegistry &get_registry() {
    return widget_registry;
  }

  void update() {
    const int widget_count = widget_registry.get_count();

    for (int i = 0; i < widget_count; i++) {
      IWidget *widget = widget_registry.get(i);

      if (widget)
        widget->update();
    }
  }

  void draw() {
    const int widget_count = widget_registry.get_count();

    for (int i = 0; i < widget_count; i++) {
      IWidget *widget = widget_registry.get(i);

      if (widget)
        widget->draw();
    }
  }

  void handle_mouse_event(const MouseEvent &event) {
    for (int i = 0; i < widget_registry.get_count(); i++) {
      IWidget *widget = widget_registry.get(i);

      // nothing for now
      //if (widget)
        //widget->handle_mouse_event(event);
    }
  }
};
