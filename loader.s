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

global timer_stub
timer_stub:
    pushad
    call c_timer_handler
    
    ; Send EOI (End of Interrupt) to Master PIC
    mov al, 0x20
    out 0x20, al
    
    popad
    iret

; Fallback for any vector we don't have a real handler for yet.
; Sending EOI to both PICs is harmless even if the interrupt wasn't
; PIC-sourced (e.g. a CPU exception) - it just prevents an unhandled
; vector from cascading into a double/triple fault while we're still
; bringing the rest of the ISR table online.

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
    
    ; mov byte [0xB8000], 'X'
    ; mov byte [0xB8001], 0x04

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

