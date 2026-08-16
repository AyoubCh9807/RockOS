#pragma once
#include "../core/asm.hpp"

inline void init_serial() {
    Asm::outb(0x3f8 + 1, 0x00);    // Disable interrupts
    Asm::outb(0x3f8 + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    Asm::outb(0x3f8 + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    Asm::outb(0x3f8 + 1, 0x00);    //                   (hi byte)
    Asm::outb(0x3f8 + 3, 0x03);    // 8 bits, no parity, one stop bit
    Asm::outb(0x3f8 + 2, 0xC7);    // Enable FIFO, clear with 14-byte threshold
    Asm::outb(0x3f8 + 4, 0x0B);    // Enable IRQs, RTS/DSR set
}

inline void serial_putchar(char a) {
    // Wait until the transmit holding register is empty
    while ((Asm::inb(0x3f8 + 5) & 0x20) == 0);
    Asm::outb(0x3f8, a);
}

inline void serial_write(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        serial_putchar(str[i]);
    }
}
