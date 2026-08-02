#include "../utils/string_utils.hpp"
#include "disk.hpp"
#include "file_entry.hpp"

constexpr size_t SECTOR_SIZE = 512;
constexpr size_t MAX_LBA = 11;
constexpr size_t FILES_PER_BLOCK = 11;
constexpr int FILE_LBA_START = 1;
constexpr int DIR_LBA_START = 6;

class FileSystem {
private:
  Disk &disk;

public:
  FileSystem(Disk &d) : disk(d) {}

  // Initialize the disk (write initial superblock)
  void format() {
    u8 sector_buffer[512];
    for (int i = 0; i < 512; i++)
      sector_buffer[i] = 0;

    // Superblock
    u32 *magic_ptr = (u32 *)sector_buffer;
    *magic_ptr = 0x524F434B; // "ROCK"
    disk.write_sector(0, sector_buffer);

    // Zero the magic number back out so we don't accidentally write
    // it into every directory sector too
    *magic_ptr = 0;

    // Zero every directory sector so is_used starts false everywhere
    for (u32 dir_sector = DIR_LBA_START; dir_sector < MAX_LBA; dir_sector++) {
      disk.write_sector(dir_sector, sector_buffer);
    }
  }
  // Create a file or directory
  void create(const char *name, bool is_dir) {

  };

  // Read file content into a buffer using your disk.read_sector()
  void read_file(const char *name, u8 *output_buffer) {

    u8 sector_buffer[SECTOR_SIZE];

    for (int current_sector = DIR_LBA_START; current_sector < MAX_LBA;
         current_sector++) {
      disk.read_sector(current_sector, sector_buffer);
      FileEntry *file_entries = (FileEntry *)sector_buffer;
      for (int i = 0; i < FILES_PER_BLOCK; i++) {
        if (file_entries[i].is_used &&
            StringUtils::strcmp(file_entries[i].name, name) == 0) {
          disk.read_sector(file_entries[i].start_block, output_buffer);
          return;
        }
      }
    }
  };

  // Write data to a file, allocating sectors via disk.write_sector()
  // A directory is a collection of metadata, so we write
  // the file metadata in a directory block, and we write the actual
  // file content in a seperate file block
  void write_file(char *name, const u8 *data, u32 size) {
    u8 temp_buffer[SECTOR_SIZE];

    for (int current_sector = DIR_LBA_START; current_sector < MAX_LBA;
         current_sector++) {
      disk.read_sector(current_sector, temp_buffer);
      FileEntry *file_entries = (FileEntry *)temp_buffer;
      for (int i = 0; i < FILES_PER_BLOCK; i++) {
        if (!file_entries[i].is_used) {

          u32 data_block = FILE_LBA_START +
                           (current_sector - DIR_LBA_START) * FILES_PER_BLOCK +
                           i;
          StringUtils::strcpy(file_entries[i].name, name);
          file_entries[i].start_block = data_block;
          file_entries[i].is_used = true;
          file_entries[i].size = size;
          disk.write_sector(current_sector, temp_buffer);
          disk.write_sector(data_block, data);
          return;
        }
      }
    }
  };
};
