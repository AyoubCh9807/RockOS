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

    ; Ask GRUB for a 1024x768 32-bit framebuffer.
    dw 5
    dw 0
    dd 20
    dd 1024
    dd 768
    dd 32

    ; Multiboot2 tags must be aligned to 8 bytes.
    align 8

    ; Every Multiboot2 header ends with an 8-byte end tag.
    dw 0
    dw 0
    dd 8

multiboot_header_end


section .text

bits 32

global loader

extern kernel_main
extern c_timer_handler
extern c_keyboard_handler


loader:

    ; GRUB gives us the address of the Multiboot2 information
    ; structure in EBX. Save it before changing registers.
    mov [multiboot_info], ebx

    ; Temporary stack while we're still in 32-bit mode.
    mov esp, stack_space + KERNEL_STACK_SIZE

    ; Start with completely empty page tables.
    mov edi, page_tables
    mov ecx, PAGE_TABLE_SIZE / 4

    xor eax, eax
    rep stosd

    ; PML4[0] points to our PDPT.
    ; 0x03 = present + writable.
    mov eax, pdpt
    or eax, 0x03
    mov [pml4], eax

    ; Each PDPT entry points to one page directory.
    mov eax, page_directory0
    or eax, 0x03
    mov [pdpt], eax

    mov eax, page_directory1
    or eax, 0x03
    mov [pdpt + 8], eax

    mov eax, page_directory2
    or eax, 0x03
    mov [pdpt + 16], eax

    mov eax, page_directory3
    or eax, 0x03
    mov [pdpt + 24], eax

    ; Identity-map the first 4 GiB using 2 MiB pages.
    ;
    ; 4 page directories × 512 entries × 2 MiB = 4 GiB.
    ;
    ; 0x83 means:
    ; bit 0 = present
    ; bit 1 = writable
    ; bit 7 = 2 MiB page
    mov edi, page_directory0
    mov eax, 0x00000083
    mov ecx, 512 * 4

.map_pd:

    mov [edi], eax

    add eax, 0x200000
    add edi, 8

    loop .map_pd

    ; Tell the CPU where our PML4 is.
    mov eax, pml4
    mov cr3, eax

    ; Enable PAE.
    mov eax, cr4
    or eax, (1 << 5)
    mov cr4, eax

    ; Enable long mode through the EFER MSR.
    ; Bit 8 of EFER is Long Mode Enable.
    mov ecx, 0xC0000080

    rdmsr

    or eax, (1 << 8)

    wrmsr

    ; Enable paging.
    mov eax, cr0
    or eax, (1 << 31)
    mov cr0, eax

    ; Load the GDT containing our 64-bit code segment.
    lgdt [gdt64.pointer]

    ; The far jump reloads CS and enters the 64-bit code segment.
    jmp CODE_SEG:long_mode


bits 64

long_mode:

    ; Load the kernel data segment.
    mov ax, DATA_SEG

    mov ds, ax
    mov es, ax
    mov ss, ax

    ; Switch to our 64-bit stack.
    mov rsp, stack_space + KERNEL_STACK_SIZE

    ; Pass the Multiboot2 information address to kernel_main.
    ;
    ; System V AMD64 ABI:
    ; RDI = first argument.
    ;
    ; The Multiboot information structure is currently below
    ; 4 GiB, so loading the 32-bit value into EDI is enough.
    mov edi, [multiboot_info]

    ; Finally enter the C++ kernel.
    call kernel_main


.hang:

    cli
    hlt

    jmp .hang


; These functions are referenced by the C++ IDT code.
; Interrupts must remain disabled until the IDT and PIC
; have been configured by the kernel.

global default_stub
global timer_stub
global keyboard_stub


default_stub:

    cli
    hlt

    jmp default_stub


timer_stub:

    ; Save every general-purpose register that C++ could change.
    ; An interrupt can happen in the middle of any kernel code,
    ; so we have to restore the interrupted state exactly.
    push rax
    push rcx
    push rdx
    push rbx
    push rbp
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    call c_timer_handler

    ; Tell the PIC that IRQ0 has been handled.
    mov al, 0x20
    out 0x20, al

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rbp
    pop rbx
    pop rdx
    pop rcx
    pop rax

    iretq


keyboard_stub:

    ; Preserve the interrupted CPU state before entering C++.
    push rax
    push rcx
    push rdx
    push rbx
    push rbp
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    ; C++ reads the keyboard scancode from port 0x60.
    call c_keyboard_handler

    ; Tell the master PIC that IRQ1 has been handled.
    mov al, 0x20
    out 0x20, al

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rbp
    pop rbx
    pop rdx
    pop rcx
    pop rax

    iretq


; A simple GDT with a null descriptor, 64-bit code segment,
; and data segment.

align 8

CODE_SEG equ 0x08
DATA_SEG equ 0x10


gdt64:

.null:

    dq 0


.code:

    ; 64-bit kernel code segment.
    dq 0x00AF9A000000FFFF


.data:

    ; Kernel data segment.
    dq 0x00AF92000000FFFF


.pointer:

    dw gdt64.pointer - gdt64 - 1
    dq gdt64


section .bss

align 4096

; These are temporary page tables used while entering long mode.
;
; PML4
;   -> PDPT
;       -> 4 page directories
;           -> 512 × 2 MiB pages each
;
; Together they identity-map the first 4 GiB.

page_tables:

pml4:
    resq 512

pdpt:
    resq 512

page_directory0:
    resq 512

page_directory1:
    resq 512

page_directory2:
    resq 512

page_directory3:
    resq 512


PAGE_TABLE_SIZE equ $ - page_tables


; GRUB's Multiboot2 information structure address.

align 8

multiboot_info:

    resq 1


; Temporary kernel stack.

align 16

KERNEL_STACK_SIZE equ 16384

stack_space:

    resb KERNEL_STACK_SIZE
