#!/bin/bash
set -e

echo "Compiling OS..."

# Compile assembly loader from boot/
nasm -f elf32 boot/loader.s -o loader.o

# Compile C++ kernel with strict freestanding flags and include path set to kernel/
g++ -m32 -c kernel/core/kernel.cpp -o kernel.o \
    -Ikernel \
    -ffreestanding \
    -O2 \
    -Wall \
    -Wextra \
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
    -mgeneral-regs-only

# Link binaries using the custom linker script from boot/
ld -m elf_i386 -T boot/link.ld -o my_kernel.bin loader.o kernel.o --no-warn-rwx-segments

# Copy to ISO directory and recreate ISO
cp my_kernel.bin isodir/boot/
grub-mkrescue -o my_os.iso isodir >/dev/null 2>&1

# Launch OS via QEMU with interrupt/CPU reset logging enabled
qemu-system-i386 -enable-kvm -cdrom my_os.iso -boot d -display gtk -d int,cpu_reset -D qemu.log
