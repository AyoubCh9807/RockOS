#pragma once
#include "../shared/types.hpp"
#include "../data/colors.hpp"

class Widget {
protected:
  int x = 0;
  int y = 0;

  int width = 80;
  int height = 80;

  int min_width = 80;
  int min_height = 80;

  int max_width = 480;
  int max_height = 480;

  bool visible = true;

  int z_index = 0;

  u32 background_color = Colors::RED;
  u32 border_color = Colors::GOLD;
  u32 text_color = Colors::WHITE;

  int padding = 0;

public:
  virtual ~Widget() = default;

  virtual void draw() = 0;
  virtual void update() = 0;
};
