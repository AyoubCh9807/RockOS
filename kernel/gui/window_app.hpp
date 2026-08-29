#pragma once

#include "../shared/key_event.hpp"

class Window;

/* Same philosophy as ICliApp, the app does not own an event loop or
   touch the screen directly, the manager drives it and hands it a
   Window to draw into. In Phase 2, this interface is what gets moved
   into a real process, on_draw/on_key become the handlers on the
   other side of a syscall boundary instead of direct virtual calls,
   but the shape of what an app needs to do stays the same. */
class IWindowApp {
public:
  virtual const char *name() const = 0;

  // Called once, right after the WindowManager creates the window.
  virtual void on_create(Window &win) {}

  /* Called every time the window needs to redraw, draw the app's
     entire contents into win here, WindowManager composites it onto
     the real screen right after this returns. */
  virtual void on_draw(Window &win) {}

  // Called when this window is focused and a key is pressed.
  virtual void on_key(Window &win, const KeyEvent &ev) {}

  // Called right before the window is destroyed.
  virtual void on_destroy(Window &win) {}

  virtual ~IWindowApp() = default;
};
