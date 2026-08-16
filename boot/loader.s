section .multiboot
align 8

MULTIBOOT2_MAGIC equ 0xE85250D6
ARCHITECTURE      equ 0

HEADER_LENGTH equ multiboot_header_end - multiboot_header_start
CHECKSUM      equ -(MULTIBOOT2_MAGIC + ARCHITECTURE + HEADER_LENGTH)

multiboot_header_start:

    dd MULTIBOOT2_MAGIC
    dd ARCHITECTURE
    dd HEADER_LENGTH
    dd CHECKSUM

    ; Framebuffer request
    dw 5
    dw 0
    dd 20
    dd 640
    dd 480
    dd 32

    dd 0

    ; End tag
    dw 0
    dw 0
    dd 8

multiboot_header_end:
section .text

global loader

extern kernel_main
extern c_timer_handler
extern c_keyboard_handler


global timer_stub
timer_stub:
    pushad
    call c_timer_handler

    mov al, 0x20
    out 0x20, al

    popad
    iret


global keyboard_stub
keyboard_stub:
    pushad
    call c_keyboard_handler

    mov al, 0x20
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

    ; Multiboot2 gives the information structure in EBX
    push ebx
    call kernel_main
    add esp, 4

.hang:
    cli
    hlt
    jmp .hang


section .bss

align 16

KERNEL_STACK_SIZE equ 16384

stack_space:
    resb KERNEL_STACK_SIZE
