ENTRY(loader)

SECTIONS
{
    . = 1M;

    /*
     * Multiboot2 header MUST be near the beginning
     * of the kernel image.
     */
    .multiboot ALIGN(8) :
    {
        KEEP(*(.multiboot))
    }

    .text ALIGN(16) :
    {
        *(.text)
    }

    .rodata ALIGN(16) :
    {
        *(.rodata*)
    }

    .data ALIGN(16) :
    {
        *(.data)

        __init_array_start = .;
        KEEP(*(SORT(.init_array*)))
        __init_array_end = .;

        __ctors_start = .;
        KEEP(*(SORT(.ctors*)))
        KEEP(*(.ctors))
        __ctors_end = .;
    }

    .bss ALIGN(16) :
    {
        *(.bss)
        *(.bss.*)
        *(COMMON)
    }

    . = ALIGN(4096);
    __kernel_end = .;
}
