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
    AMP_STACK,
    EQUALIZER,
    GUITAR,
    HEADPHONE_SKULL,
    VINYL,
    SPEAKER_WALL,
    MOSHPIT,
    STAGE_LIGHTS,
    RAVEN,
    BARBED_WIRE,
    DARK_CASTLE,
    DRUM_KIT,
    MIC_STAND,
    NOTES_RAIN,
    FIRST = CHAINS,
    LAST = NOTES_RAIN,
    COUNT = LAST + 1
  };

  static inline WallpaperVariant current_wallpaper = WallpaperVariant::CHAINS;
  static constexpr u32 WALLPAPER_COUNT = static_cast<u32>(WallpaperVariant::COUNT);

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
    case WallpaperVariant::AMP_STACK:
      draw_amp_stack();
      break;
    case WallpaperVariant::EQUALIZER:
      draw_equalizer();
      break;
    case WallpaperVariant::GUITAR:
      draw_guitar();
      break;
    case WallpaperVariant::HEADPHONE_SKULL:
      draw_headphone_skull();
      break;
    case WallpaperVariant::VINYL:
      draw_vinyl();
      break;
    case WallpaperVariant::SPEAKER_WALL:
      draw_speaker_wall();
      break;
    case WallpaperVariant::MOSHPIT:
      draw_moshpit();
      break;
    case WallpaperVariant::STAGE_LIGHTS:
      draw_stage_lights();
      break;
    case WallpaperVariant::RAVEN:
      draw_raven();
      break;
    case WallpaperVariant::BARBED_WIRE:
      draw_barbed_wire();
      break;
    case WallpaperVariant::DARK_CASTLE:
      draw_dark_castle();
      break;
    case WallpaperVariant::DRUM_KIT:
      draw_drum_kit();
      break;
    case WallpaperVariant::MIC_STAND:
      draw_mic_stand();
      break;
    case WallpaperVariant::NOTES_RAIN:
      draw_notes_rain();
      break;
    default:
      break;
    }
  }

public:
  Wallpaper() {}

  void draw() { draw_selected_wallpaper(); }
};
