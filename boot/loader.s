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
extern c_timer_handler
extern c_keyboard_handler

global timer_stub
timer_stub:
    pushad
    call c_timer_handler
    
    ; Send EOI (End of Interrupt) to Master PIC
    mov al, 0x20
    out 0x20, al
    
    popad
    iret

global keyboard_stub
keyboard_stub:
    pushad              ; save registers
    call c_keyboard_handler
    mov al, 0x20        ; send EOI (End of Interrupt) to Master PIC
    out 0x20, al
    popad
    iret

global default_stub
default_stub:
    pushad
    mov al, 0x20
    out 0x20, al
    out 0xA0, al
    popad
    iret

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
