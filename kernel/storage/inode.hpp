#pragma once

#include "../shared/types.hpp"
#include "layout.hpp"

struct Inode {

  u32 id;
  u32 size;
  bool is_directory;
  u32 direct_blocks[DIRECT_BLOCKS];
  u32 indirect_block;
  u32 parent_inode;
  bool used;
};
