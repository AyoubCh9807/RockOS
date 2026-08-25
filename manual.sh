rm -f *.o my_kernel.bin my_os.iso isodir/boot/my_kernel.bin
nasm -f elf64 boot/loader.s -o loader.o && echo "ASM OK"
g++ -m64 -c kernel/core/kernel.cpp -o kernel.o -Ikernel -ffreestanding -O2 -fno-exceptions -fno-rtti -fno-use-cxa-atexit -fno-stack-protector -nostdlib -fno-builtin -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mno-80387 -mgeneral-regs-only && echo "KERNEL.O OK"
g++ -m64 -c kernel/memory/new_delete.cpp -o new_delete.o -Ikernel -ffreestanding -O2 -fno-exceptions -fno-rtti -fno-use-cxa-atexit -fno-stack-protector -nostdlib -fno-builtin -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mno-80387 -mgeneral-regs-only && echo "NEW_DELETE.O OK"
ld -m elf_x86_64 -T boot/link.ld -o my_kernel.bin loader.o kernel.o new_delete.o --no-warn-rwx-segments && echo "LINK OK"
strings my_kernel.bin | grep TIMER_HANDLER_V2

cp my_kernel.bin isodir/boot/
grub-mkrescue -o my_os.iso isodir
qemu-system-x86_64 -cdrom my_os.iso -boot d -drive file=disk.img,format=raw,if=ide,index=0,media=disk -display gtk -no-reboot -no-shutdown -d int,cpu_reset -D qemu.log
