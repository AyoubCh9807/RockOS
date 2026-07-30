ALIGN 4
MODULE_ALIGN   equ  1<<0             
MEM_INFO       equ  1<<1             
FLAGS          equ  MODULE_ALIGN | MEM_INFO  
MAGIC          equ  0x1BADB002       
CHECKSUM       equ  -(MAGIC + FLAGS)     

section .multiboot
    align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

section .text
global loader
extern kernel_main                   

loader:
    mov esp, stack_space + KERNEL_STACK_SIZE
    call kernel_main

.hang:
    cli
    hlt
    jmp .hang

section .bss
align 16
KERNEL_STACK_SIZE equ 16384          
stack_space:
    resb KERNEL_STACK_SIZE

