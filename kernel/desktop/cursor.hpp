#pragma once

#include "cursor_bitmaps.hpp"

namespace Cursor {

// Arrow horns ibeam crosshair hourglass pick neck glove_point

enum class CursorVariant {
  ARROW,
  HORNS,
  IBEAM,
  CROSSHAIR,
  HOURGLASS,
  PICK,
  NECK_DIAGONAL,
  GLOVE_POINT,
  FIRST = ARROW,
  LAST = GLOVE_POINT,
  COUNT = LAST + 1
};

static CursorVariant current = CursorVariant::ARROW;

static void select_next_cursor() {
  int next_val = static_cast<int>(current) + 1;
  if (next_val >= static_cast<int>(CursorVariant::COUNT)) {
    current = CursorVariant::ARROW;
  } else {
    current = static_cast<CursorVariant>(next_val);
  }
}

static RockCursors::CursorBitmap const *get_current_cursor_bitmap() {
  return RockCursors::ALL[static_cast<int>(current)];
}

static constexpr auto WIDTH = RockCursors::SIZE;
static constexpr auto HEIGHT = RockCursors::SIZE;
} // namespace Cursor
