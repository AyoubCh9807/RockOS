#pragma once

#include "../kernel/core/kernel.hpp"

struct Framebuffer {
  u8 *address;
  u32 pitch;
  u32 width;
  u32 height;
  u8 bpp;
  u8 type;
  bool valid;
};

namespace Multiboot2 {

inline Framebuffer framebuffer = {nullptr, 0, 0, 0, 0, 0, false};

constexpr u32 BOOTLOADER_MAGIC = 0x36D76289;

enum TagType : u32 {
  END = 0,
  CMDLINE = 1,
  BOOT_LOADER_NAME = 2,
  MODULE = 3,
  BASIC_MEMINFO = 4,
  BOOTDEV = 5,
  MMAP = 6,
  VBE = 7,
  FRAMEBUFFER = 8
};

struct __attribute__((packed)) Info {
  u32 total_size;
  u32 reserved;
};

struct __attribute__((packed)) Tag {
  u32 type;
  u32 size;
};

struct __attribute__((packed)) FramebufferTag {
  u32 type;
  u32 size;

  u64 framebuffer_addr;
  u32 framebuffer_pitch;
  u32 framebuffer_width;
  u32 framebuffer_height;
  u8 framebuffer_bpp;
  u8 framebuffer_type;
  u16 reserved;
};

inline static void fill_tags(u64 multiboot_addr) {
  Info *info = reinterpret_cast<Info *>(multiboot_addr);

  u64 offset = 8;

  while (offset < info->total_size) {
    Tag *tag =
        reinterpret_cast<Tag *>(multiboot_addr + offset);

    if (tag->type == FRAMEBUFFER) {
      FramebufferTag *fb =
          reinterpret_cast<FramebufferTag *>(tag);

      framebuffer.address =
          reinterpret_cast<u8 *>(fb->framebuffer_addr);

      framebuffer.pitch = fb->framebuffer_pitch;
      framebuffer.width = fb->framebuffer_width;
      framebuffer.height = fb->framebuffer_height;
      framebuffer.bpp = fb->framebuffer_bpp;
      framebuffer.type = fb->framebuffer_type;
      framebuffer.valid = true;
    }

    if (tag->type == END) {
      break;
    }

    offset += (tag->size + 7) & ~7;
  }
}

} // namespace Multiboot2
