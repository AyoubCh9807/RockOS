set -e
echo "Compiling OS..."

# Compile assemblt
nasm -f elf32 loader.s -o loader.o

# Compile C++ kernel
g++ -m32 -c kernel.cpp -o kernel.o -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti

# Link binaries using a custom script
ld -m elf_i386 -T link.ld -o my_kernel.bin loader.o kernel.o

# Copy to ISO directory and recreate ISO
cp my_kernel.bin isodir/boot/
grub-mkrescue -o my_os.iso isodir

# Launch OS via QEMU using the ISO file
qemu-system-i386 -enable-kvm -cdrom my_os.iso -display gtk
