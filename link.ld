ENTRY(loader)

SECTIONS
{
    . = 1M;

    .text : {
        *(.multiboot)
        *(.text)
    }

    .rodata : {
        *(.rodata*)
    }

  .data : {
        *(.data)
        
        /* Modern constructors (.init_array) */
        __init_array_start = .;
        KEEP(*(SORT(.init_array*)))
        __init_array_end = .;

        /* Legacy constructors (.ctors) */
        __ctors_start = .;
        KEEP(*(SORT(.ctors*)))
        KEEP(*(.ctors))
        __ctors_end = .;
    }


    .bss : {
        *(.bss)
    }
}
