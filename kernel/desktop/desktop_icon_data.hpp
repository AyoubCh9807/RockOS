#pragma once

#include "../shared/types.hpp"

// Shared by IconBitmaps and RockIcons (and anything else that wants to put
// an icon on the desktop). Kept in its own header so both icon sets can
// include it without either one owning the definition.
struct DesktopIconData {
  const char *label;

  const u32 *pixels;
  u32 width;
  u32 height;
};
