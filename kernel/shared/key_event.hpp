#pragma once
#include "types.hpp"

static constexpr u8 KEY_ARROW_LEFT = 128;
static constexpr u8 KEY_ARROW_RIGHT = 129;
static constexpr u8 KEY_ARROW_UP = 130;
static constexpr u8 KEY_ARROW_DOWN = 131;

enum class KeyType : u8 {
  Char,
  BackSpace,
  Enter,
  ArrowUp,
  ArrowDown,
  ArrowLeft,
  ArrowRight,
  None
};

struct KeyEvent {
  KeyType keytype;
  int scancode;
};
