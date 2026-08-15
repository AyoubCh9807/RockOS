#pragma once

#include "../shared/types.hpp"

struct __attribute__((packed)) MultibootInfoRaw {
    u32 flags;

    u32 mem_lower;
    u32 mem_upper;

    u32 boot_device;
    u32 cmdline;

    u32 mods_count;
    u32 mods_addr;

    u8 syms[16];

    u32 mmap_length;
    u32 mmap_addr;

    u32 drives_length;
    u32 drives_addr;

    u32 config_table;
    u32 boot_loader_name;
    u32 apm_table;

    u32 vbe_control_info;
    u32 vbe_mode_info;

    u16 vbe_mode;
    u16 vbe_interface_seg;
    u16 vbe_interface_off;
    u16 vbe_interface_len;
};
