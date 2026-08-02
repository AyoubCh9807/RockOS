#include "../shared/types.hpp"

constexpr int MAX_FILE_ENTRY_NAME_LENGTH = 32;

struct FileEntry {

  char name[MAX_FILE_ENTRY_NAME_LENGTH];
  u32 size;
  u32 start_block;
  bool is_dir;
  bool is_used;
};
