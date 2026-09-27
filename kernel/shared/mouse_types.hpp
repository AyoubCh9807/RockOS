#pragma once

enum class MouseButton { LEFT_BUTTON, RIGHT_BUTTON, MIDDLE_BUTTON, NONE };
enum class MouseEventType { MOVE, PRESS, RELEASE, NONE };

struct MouseEvent {
  MouseButton button_type;   // eg LEFT_CLICK / RIGHT_CLICK / MIDDLE_CLICK
  MouseEventType event_type; // eg MOVE / PRESS / RELEASE
  int tick = 0;
};


