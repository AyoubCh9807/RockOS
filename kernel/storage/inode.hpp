#pragma once

#include "../shared/types.hpp"

constexpr int DIRECT_BLOCKS = 12;

struct Inode {

  u32 id;
  u32 size;
  bool is_directory;
  u32 direct_blocks[DIRECT_BLOCKS];
  u32 indirect_block;
  u32 parent_inode;
  bool used;
};
