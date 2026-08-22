#pragma once

#include "../random/random.hpp"
#include "../shared/types.hpp"

// RockOS terminal color palette
// Values are 24-bit RGB: 0xRRGGBB

namespace Colors {

// Grayscale

constexpr u32 BLACK = 0x000000;
constexpr u32 DARK_GRAY = 0x202020;
constexpr u32 GRAY = 0x404040;
constexpr u32 SLATE = 0x606060;
constexpr u32 SILVER = 0xA0A0A0;
constexpr u32 LIGHT_GRAY = 0xC0C0C0;
constexpr u32 WHITE = 0xFFFFFF;
constexpr u32 OFF_WHITE = 0xF0F0F0;

// Reds

constexpr u32 DARK_RED = 0x800000;
constexpr u32 RED = 0xFF0000;
constexpr u32 LIGHT_RED = 0xFF4040;
constexpr u32 CRIMSON = 0xDC143C;
constexpr u32 CORAL = 0xFF6B6B;
constexpr u32 SALMON = 0xFA8072;
constexpr u32 ROSE = 0xFF007F;
constexpr u32 PINK = 0xFFC0CB;

// Oranges / Yellows

constexpr u32 DARK_ORANGE = 0xCC5500;
constexpr u32 ORANGE = 0xFFA500;
constexpr u32 LIGHT_ORANGE = 0xFFB347;
constexpr u32 PEACH = 0xFFDAB9;
constexpr u32 GOLD = 0xFFD700;
constexpr u32 YELLOW = 0xFFFF00;
constexpr u32 LEMON = 0xFFF44F;
constexpr u32 AMBER = 0xFFBF00;

// Greens

constexpr u32 DARK_GREEN = 0x006400;
constexpr u32 GREEN = 0x00FF00;
constexpr u32 FOREST = 0x228B22;
constexpr u32 LIME = 0x32CD32;
constexpr u32 MINT = 0x98FF98;
constexpr u32 SPRING = 0x00FF7F;
constexpr u32 OLIVE = 0x808000;
constexpr u32 EMERALD = 0x50C878;

// Cyans / Teals

constexpr u32 DARK_CYAN = 0x008B8B;
constexpr u32 CYAN = 0x00FFFF;
constexpr u32 LIGHT_CYAN = 0x80FFFF;
constexpr u32 TEAL = 0x008080;
constexpr u32 AQUA = 0x00FFFF;
constexpr u32 TURQUOISE = 0x40E0D0;
constexpr u32 MINT_CYAN = 0x98FFF0;
constexpr u32 SEAFOAM = 0x71EFA3;

// Blues

constexpr u32 DARK_BLUE = 0x000080;
constexpr u32 BLUE = 0x0000FF;
constexpr u32 LIGHT_BLUE = 0x4040FF;
constexpr u32 SKY_BLUE = 0x87CEEB;
constexpr u32 AZURE = 0x007FFF;
constexpr u32 ROYAL_BLUE = 0x4169E1;
constexpr u32 NAVY = 0x000080;
constexpr u32 CORNFLOWER = 0x6495ED;

// Purples / Magentas

constexpr u32 DARK_PURPLE = 0x4B0082;
constexpr u32 PURPLE = 0x800080;
constexpr u32 VIOLET = 0x8A2BE2;
constexpr u32 LAVENDER = 0xB57EDC;
constexpr u32 MAGENTA = 0xFF00FF;
constexpr u32 HOT_PINK = 0xFF69B4;
constexpr u32 PLUM = 0xDDA0DD;
constexpr u32 FUCHSIA = 0xFF00AA;

constexpr u32 PALETTE[] = {BLACK,       DARK_GRAY,  GRAY,         SLATE,
                           SILVER,      LIGHT_GRAY, WHITE,        OFF_WHITE,

                           DARK_RED,    RED,        LIGHT_RED,    CRIMSON,
                           CORAL,       SALMON,     ROSE,         PINK,

                           DARK_ORANGE, ORANGE,     LIGHT_ORANGE, PEACH,
                           GOLD,        YELLOW,     LEMON,        AMBER,

                           DARK_GREEN,  GREEN,      FOREST,       LIME,
                           MINT,        SPRING,     OLIVE,        EMERALD,

                           DARK_CYAN,   CYAN,       LIGHT_CYAN,   TEAL,
                           AQUA,        TURQUOISE,  MINT_CYAN,    SEAFOAM,

                           DARK_BLUE,   BLUE,       LIGHT_BLUE,   SKY_BLUE,
                           AZURE,       ROYAL_BLUE, NAVY,         CORNFLOWER,

                           DARK_PURPLE, PURPLE,     VIOLET,       LAVENDER,
                           MAGENTA,     HOT_PINK,   PLUM,         FUCHSIA};

constexpr int PALETTE_SIZE = sizeof(PALETTE) / sizeof(PALETTE[0]);

static const u32 pick_random_color() {
  int random_index = Random::next();
  while(PALETTE[random_index % PALETTE_SIZE] == BLACK) random_index = Random::next();
  return PALETTE[random_index % PALETTE_SIZE];
}

} // namespace Colors
