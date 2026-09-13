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
extern c_mouse_handler
extern c_pagefault_handler
extern c_gpfault_handler
extern next_resume_rsp
extern debug_resume_check
extern next_resume_cr3

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
    ; stack_space is 16-byte aligned (see .bss) and KERNEL_STACK_SIZE
    ; is a multiple of 16, so rsp is 16-byte aligned here, as the
    ; System V AMD64 ABI requires at a call site.
    mov rsp, stack_space + KERNEL_STACK_SIZE

    ; Enable SSE before any C++ code runs, since the compiler is free
    ; to emit SSE/XMM instructions (float/double math, struct copies,
    ; vectorized code) anywhere, including in kernel_main's prologue.
    ;
    ; CR0.EM (bit 2) = 0  -> disable "emulate FPU", i.e. allow real
    ;                        x87/SSE instructions instead of faulting.
    ; CR0.MP (bit 1) = 1  -> monitor co-processor, required alongside EM=0.
    mov rax, cr0
    and ax, 0xFFFB
    or ax, 0x2
    mov cr0, rax

    ; CR4.OSFXSR    (bit 9)  -> OS supports FXSAVE/FXRSTOR, unlocks SSE.
    ; CR4.OSXMMEXCPT (bit 10) -> OS handles unmasked SIMD FP exceptions (#XM).
    mov rax, cr4
    or rax, (3 << 9)
    mov cr4, rax

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


; These functions are referenced by the C++ IDT code (kernel/core/idt.hpp).
; Interrupts must remain disabled until the IDT and PIC
; have been configured by the kernel.
;
; Every stub below saves/restores the SSE/x87 state with FXSAVE/FXRSTOR
; in addition to the GPRs. This is required once SSE is enabled: if an
; interrupt fires while kernel code (e.g. AI model inference) has live
; values in xmm0-xmm15, and the handler itself - or code the compiler
; generates for it - touches SSE state, it will silently clobber those
; registers without this save/restore.
;
; FXSAVE/FXRSTOR require a 512-byte buffer that is 16-byte aligned.
; We reserve the space first, before pushing GPRs, so the alignment
; math stays simple.

global default_stub
global timer_stub
global keyboard_stub
global mouse_stub
global pagefault_stub
global gpfault_stub


default_stub:

    cli
    hlt

    jmp default_stub


timer_stub:
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

    ; rdi must be captured HERE, before the fxsave buffer is carved out,
    ; so it still points at the top of the 15-GPR block this handler's
    ; C++ side expects.
    mov rdi, rsp

    ; Protect the interrupted task's SSE/x87 state strictly around the
    ; C call. This buffer must NOT wrap the CR3/RSP switch below:
    ; next_resume_rsp is computed elsewhere assuming the stack frame
    ; ends exactly after the 15 GPR pushes above. Wrapping fxsave/fxrstor
    ; around the whole stub (as an earlier version of this file did)
    ; shifts that frame by 512 bytes relative to what the resume logic
    ; expects, so the pops below read the wrong memory and iretq faults
    ; on a garbage return frame. Bracketing tightly around just the call
    ; keeps rsp exactly where it was before this block once we're done,
    ; leaving the switch logic untouched.
    sub rsp, 512
    fxsave [rsp]

    call c_timer_handler

    fxrstor [rsp]
    add rsp, 512

    mov al, 0x20
    out 0x20, al

    mov rax, [next_resume_cr3]
    mov cr3, rax

    mov rax, [next_resume_rsp]
    mov rsp, rax

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

    ; preserve the interrupted cpu state before entering c++.
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

    sub rsp, 512
    fxsave [rsp]

    ; c++ reads the keyboard scancode from port 0x60.
    call c_keyboard_handler

    fxrstor [rsp]
    add rsp, 512

    ; tell the master pic that irq1 has been handled.
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


mouse_stub:
    ; preserve the interrupted cpu state before entering c++.
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

    sub rsp, 512
    fxsave [rsp]

    ; c++ reads the mouse event from port 0x60.
    call c_mouse_handler

    fxrstor [rsp]
    add rsp, 512

    ; tell both the master and the slave pics that irq12 has been handled.
    mov al, 0x20
    out 0xA0, al
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


; Page fault (#PF, vector 14). The CPU pushes a 32-bit (zero-extended
; to 64-bit) error code before this fires. We save all GPRs and the
; SSE/x87 state, pass a pointer to the GPR block (plus the error code
; sitting right above it) to C++ for logging, then halt. This does
; NOT attempt to recover/resume - it's a debugging aid until real
; fault handling exists.
pagefault_stub:

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

    ; Capture the GPR-block pointer BEFORE carving out the fxsave buffer,
    ; so it points where c_pagefault_handler expects (error code and
    ; fault RIP sitting exactly 15 and 16 u64 slots above it).
    mov rdi, rsp

    sub rsp, 512
    fxsave [rsp]

    call c_pagefault_handler

pagefault_hang:
    cli
    hlt
    jmp pagefault_hang


; General protection fault (#GP, vector 13). Same shape as the page
; fault stub above - CPU pushes an error code, we log and halt.
gpfault_stub:

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

    ; Same reasoning as pagefault_stub: capture rdi before the fxsave
    ; buffer shifts rsp, so c_gpfault_handler's saved_regs[15] (error
    ; code) lands where it's supposed to.
    mov rdi, rsp

    sub rsp, 512
    fxsave [rsp]

    call c_gpfault_handler

gpfault_hang:
    cli
    hlt
    jmp gpfault_hang


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

KERNEL_STACK_SIZE equ 65536

stack_space:

    resb KERNEL_STACK_SIZE
