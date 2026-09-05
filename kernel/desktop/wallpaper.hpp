#pragma once

#include "wallpaper_methods.hpp"

class Wallpaper {
private:
  enum class WallpaperVariant : u32 {
    CHAINS,
    SKULL,
    BAT_WINGS,
    SPIDER_WEB,
    LIGHTNING,
    DAGGER,
    COFFIN,
    INFERNO,
    BLOOD_MOON,
    BROKEN_HEART,
    FIRST = CHAINS,
    LAST = BROKEN_HEART,
    COUNT = 10
  };

  static inline WallpaperVariant current_wallpaper = WallpaperVariant::CHAINS;
  static constexpr u32 WALLPAPER_COUNT = 10;

public:
  static void select_next_wallpaper() {
    u32 next_val = static_cast<u32>(current_wallpaper) + 1;

    if (next_val >= static_cast<u32>(WallpaperVariant::COUNT)) {
      next_val = static_cast<u32>(WallpaperVariant::FIRST);
    }

    current_wallpaper = static_cast<WallpaperVariant>(next_val);
  }

  static void draw_selected_wallpaper() {
    switch (current_wallpaper) {
    case WallpaperVariant::CHAINS:
      draw_chains();
      break;
    case WallpaperVariant::SKULL:
      draw_skull();
      break;
    case WallpaperVariant::BAT_WINGS:
      draw_bat_wings();
      break;
    case WallpaperVariant::SPIDER_WEB:
      draw_spider_web();
      break;
    case WallpaperVariant::LIGHTNING:
      draw_lightning();
      break;
    case WallpaperVariant::DAGGER:
      draw_dagger();
      break;
    case WallpaperVariant::COFFIN:
      draw_coffin();
      break;
    case WallpaperVariant::INFERNO:
      draw_inferno();
      break;
    case WallpaperVariant::BLOOD_MOON:
      draw_blood_moon();
      break;
    case WallpaperVariant::BROKEN_HEART:
      draw_broken_heart();
      break;
    default:
      break;
    }
  }

public:
  Wallpaper() {}




  void draw() { draw_selected_wallpaper(); }
};
