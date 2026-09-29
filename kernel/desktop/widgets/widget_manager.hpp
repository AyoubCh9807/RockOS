#pragma once

#include "../../shared/mouse_types.hpp"
#include "iwidget.hpp"
#include "widget_registry.hpp"
#include "../../drivers/mouse.hpp"

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

  void handle_key(const KeyEvent& ev) {
      // nothing for now
      //if (widget)
        //widget->handle_mouse_event(event);
  }

  constexpr bool handle_mouse_event(const MouseEvent &ev) const {
    for (int i = 0; i < widget_registry.get_count(); i++) {
      IWidget *widget = widget_registry.get(i);

      if(widget && widget->contains(Mouse::get_x(), Mouse::get_y())) {
        widget->handle_mouse_event(ev);
        // Event was successfully passed to a widget
        return true;
      }
      // nothing for now
      //if (widget)
        //widget->handle_mouse_event(event);
    }
  }
};
