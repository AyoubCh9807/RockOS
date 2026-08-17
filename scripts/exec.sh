#!/bin/bash
set -e

echo "Compiling OS..."

# Compile assembly loader from boot/
nasm -f elf32 boot/loader.s -o loader.o

# Add these later fr
#
# -Wall \
# -Wextra \

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

g++ -m32 -c kernel/core/kernel.cpp -o kernel.o $CPPFLAGS
g++ -m32 -c kernel/memory/new_delete.cpp -o new_delete.o $CPPFLAGS

# Link kernel
ld -m elf_i386 -T boot/link.ld \
    -o my_kernel.bin \
    loader.o kernel.o new_delete.o \
    --no-warn-rwx-segments

# Copy kernel to ISO
cp my_kernel.bin isodir/boot/

# Recreate ISO
grub-mkrescue -o my_os.iso isodir >/dev/null 2>&1

# Launch OS
qemu-system-i386 \
    -enable-kvm \
    -cdrom my_os.iso \
    -boot d \
    -drive file=disk.img,format=raw,if=ide,index=0,media=disk \
    -display gtk \
    -d int,cpu_reset \
    -D qemu.log
