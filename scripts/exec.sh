#!/bin/bash
set -e

echo "Removing old build..."

rm -f *.o my_kernel.bin my_os.iso isodir/boot/my_kernel.bin

echo "Compiling OS..."

# Bootloader starts in 32-bit mode under Multiboot2.
nasm -f elf64 boot/loader.s -o loader.o

CPPFLAGS="-Ikernel \
-ffreestanding \
-O2 \
-fno-exceptions \
-fno-rtti \
-fno-use-cxa-atexit \
-fno-stack-protector \
-nostdlib \
-fno-builtin \
-mno-red-zone \
-mno-mmx \
-mno-sse \
-mno-sse2 \
-mno-80387 \
-mgeneral-regs-only"

# 64-bit kernel
g++ -m64 -c kernel/core/kernel.cpp -o kernel.o $CPPFLAGS
g++ -m64 -c kernel/memory/new_delete.cpp -o new_delete.o $CPPFLAGS

# Link as x86-64
ld -m elf_x86_64 -T boot/link.ld \
    -o my_kernel.bin \
    loader.o kernel.o new_delete.o \
    --no-warn-rwx-segments

cp my_kernel.bin isodir/boot/

grub-mkrescue -o my_os.iso isodir >/dev/null 2>&1

qemu-system-x86_64 \
    -cdrom my_os.iso \
    -boot d \
    -drive file=disk.img,format=raw,if=ide,index=0,media=disk \
    -display gtk \
    -no-reboot -no-shutdown \
    -d int,cpu_reset \
    -D qemu.log
