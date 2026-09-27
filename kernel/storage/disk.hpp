// kernel/storage/disk.hpp
#pragma once

#include "../core/asm.hpp"
#include "../shared/types.hpp"
#include "../utils/debugger.hpp"
#include "layout.hpp"

class Disk {
  constexpr static u8 ATA_SR_BSY = 0x80;
  constexpr static u8 ATA_SR_DRQ = 0x08;
  constexpr static u8 ATA_SR_ERR = 0x01;
  constexpr static u8 ATA_SR_DF = 0x20;

  constexpr static u16 ATA_IO_BASE = 0x1F0;
  constexpr static u16 ATA_CTRL_BASE =
      0x3F6; // alternate status / device control
  constexpr static u16 ATA_REG_DATA = 0x00;
  constexpr static u16 ATA_REG_SECCOUNT = 0x02;
  constexpr static u16 ATA_REG_LBA_LOW = 0x03;
  constexpr static u16 ATA_REG_LBA_MID = 0x04;
  constexpr static u16 ATA_REG_LBA_HIGH = 0x05;
  constexpr static u16 ATA_REG_DEVICE = 0x06;
  constexpr static u16 ATA_REG_STATUS = 0x07;
  constexpr static u16 ATA_REG_COMMAND = 0x07;

  // --- interrupt bookkeeping ---
  // The ISR (ata_stub -> c_disk_handler -> handle_irq) touches these.
  // They're volatile because the compiler must not assume it knows
  // their value across a "wait for the bell" loop - the ISR changes
  // them behind the CPU's back, from the compiler's point of view.
public:
  static inline Disk *active = nullptr;

private:
  volatile bool irq_pending = false; // true once the bell has rung
  volatile bool irq_error = false;   // true if the drive reported ERR/DF
  u16 *irq_read_target = nullptr;    // where to put the 256 words on a READ

  // 400ns settle delay. Reads the ALTERNATE status register (0x3F6), not the
  // main status port, so it never clears a pending IRQ flag as a side effect.
  void io_delay() {
    for (int i = 0; i < 4; i++)
      Asm::inb(ATA_CTRL_BASE);
  }

  void wait_not_busy() {
    while (Asm::inb(ATA_IO_BASE + ATA_REG_STATUS) & ATA_SR_BSY)
      ;
  }

  // Waits for BSY=0 and DRQ=1 (ready to transfer data).
  // Returns false immediately on ERR/DF instead of spinning forever.
  // Still used for the write-side "first chunk please" handshake below,
  // since the drive doesn't interrupt for that part - see write_sector_irq.
  bool wait_drq() {
    while (1) {
      u8 status = Asm::inb(ATA_IO_BASE + ATA_REG_STATUS);
      if (status & (ATA_SR_ERR | ATA_SR_DF))
        return false;
      if (!(status & ATA_SR_BSY) && (status & ATA_SR_DRQ))
        return true;
    }
  }

  void select_drive(u32 lba) {
    Asm::outb(ATA_IO_BASE + ATA_REG_DEVICE, 0xE0 | ((lba >> 24) & 0x0F));
    io_delay();
  }

  // Sit here doing nothing useful but also nothing wasteful, until the
  // doorbell rings. `hlt` puts the CPU to sleep until ANY interrupt
  // fires (timer, keyboard, disk, ...); we just loop back and check
  // our own flag each time we wake up, in case it was a different
  // interrupt that woke us up first.
  void wait_for_irq() {
    while (!irq_pending)
      Asm::halt();
    irq_pending = false;
  }

public:
  Disk() { active = this; }

  // Called from c_disk_handler (extern "C", defined below the class)
  // once per IRQ14. This is the "who rang the bell, and what did they
  // want" function. It runs inside the ISR, so keep it short.
  void handle_irq() {
    u8 status = Asm::inb(ATA_IO_BASE + ATA_REG_STATUS);

    if (status & (ATA_SR_ERR | ATA_SR_DF)) {
      irq_error = true;
      irq_pending = true;
      return;
    }

    // On a READ, the drive rings the bell to say "data's ready," and
    // DRQ is set right now - this is our one chance to pull the 256
    // words off the data port before the drive moves on.
    if (irq_read_target && (status & ATA_SR_DRQ)) {
      for (int i = 0; i < 256; i++)
        irq_read_target[i] = Asm::inw(ATA_IO_BASE + ATA_REG_DATA);
      irq_read_target = nullptr;
    }

    // On a WRITE, this same bell just means "I'm done, BSY is now 0" -
    // there's nothing left to transfer, so falling through is correct.

    irq_error = false;
    irq_pending = true;
  }

  // ---- interrupt-driven versions (the ones you actually want) ----

  bool read_sector_irq(u32 lba, u8 *buffer) {
    wait_not_busy();
    select_drive(lba);

    irq_read_target = (u16 *)buffer;
    irq_error = false;
    irq_pending = false;

    Asm::outb(ATA_IO_BASE + ATA_REG_SECCOUNT, 1);
    Asm::outb(ATA_IO_BASE + ATA_REG_LBA_LOW, (u8)lba);
    Asm::outb(ATA_IO_BASE + ATA_REG_LBA_MID, (u8)(lba >> 8));
    Asm::outb(ATA_IO_BASE + ATA_REG_LBA_HIGH, (u8)(lba >> 16));
    Asm::outb(ATA_IO_BASE + ATA_REG_COMMAND, 0x20); // READ SECTORS

    // No cli() here on purpose - we WANT interrupts enabled, that's
    // the whole point. Just go to sleep until the bell rings.
    wait_for_irq();

    return !irq_error;
  }

  bool write_sector_irq(u32 lba, const u8 *buffer) {
    if (!buffer)
      return false;

    wait_not_busy();
    select_drive(lba);

    irq_read_target = nullptr; // this is a write, not a read
    irq_error = false;
    irq_pending = false;

    Asm::outb(ATA_IO_BASE + ATA_REG_SECCOUNT, 1);
    Asm::outb(ATA_IO_BASE + ATA_REG_LBA_LOW, (u8)lba);
    Asm::outb(ATA_IO_BASE + ATA_REG_LBA_MID, (u8)(lba >> 8));
    Asm::outb(ATA_IO_BASE + ATA_REG_LBA_HIGH, (u8)(lba >> 16));
    Asm::outb(ATA_IO_BASE + ATA_REG_COMMAND, 0x30); // WRITE SECTORS

    // Quirk of real ATA hardware: for a WRITE, the drive raises DRQ to
    // ask for the first chunk of data WITHOUT an interrupt. It only
    // interrupts once it has fully swallowed and written that data.
    // So this one small wait has to stay a busy-wait - there is no
    // bell to wait for yet at this point.
    if (!wait_drq())
      return false;

    const u16 *source_ptr = (const u16 *)buffer;
    for (int i = 0; i < 256; i++)
      Asm::outw(ATA_IO_BASE + ATA_REG_DATA, source_ptr[i]);

    // NOW the drive is actually writing to the platter/flash, and
    // THIS is the part we get to do the lazy way - sleep until the
    // bell rings instead of spinning on BSY.
    wait_for_irq();

    return !irq_error;
  }

  // ---- old polling versions, kept around for debugging / comparison ----

  bool read_sector_polled(u32 lba, u8 *buffer) {
    wait_not_busy();
    select_drive(lba);

    Asm::outb(ATA_IO_BASE + ATA_REG_SECCOUNT, 1);
    Asm::outb(ATA_IO_BASE + ATA_REG_LBA_LOW, (u8)lba);
    Asm::outb(ATA_IO_BASE + ATA_REG_LBA_MID, (u8)(lba >> 8));
    Asm::outb(ATA_IO_BASE + ATA_REG_LBA_HIGH, (u8)(lba >> 16));
    Asm::outb(ATA_IO_BASE + ATA_REG_COMMAND, 0x20); // READ SECTORS

    Asm::cli();
    if (!wait_drq())
      return false;

    u16 *target_ptr = (u16 *)buffer;
    for (int i = 0; i < 256; i++)
      target_ptr[i] = Asm::inw(ATA_IO_BASE + ATA_REG_DATA);

    Asm::sti();
    return true;
  }

  bool write_sector_polled(u32 lba, const u8 *buffer) {
    if (!buffer)
      return false;

    wait_not_busy();
    select_drive(lba);

    Asm::outb(ATA_IO_BASE + ATA_REG_SECCOUNT, 1);
    Asm::outb(ATA_IO_BASE + ATA_REG_LBA_LOW, (u8)lba);
    Asm::outb(ATA_IO_BASE + ATA_REG_LBA_MID, (u8)(lba >> 8));
    Asm::outb(ATA_IO_BASE + ATA_REG_LBA_HIGH, (u8)(lba >> 16));
    Asm::outb(ATA_IO_BASE + ATA_REG_COMMAND, 0x30);

    Asm::cli();
    if (!wait_drq())
      return false;

    const u16 *source_ptr = (const u16 *)buffer;
    for (int i = 0; i < 256; i++)
      Asm::outw(ATA_IO_BASE + ATA_REG_DATA, source_ptr[i]);

    while (1) {
      u8 status = Asm::inb(ATA_IO_BASE + ATA_REG_STATUS);
      if (status & (ATA_SR_ERR | ATA_SR_DF))
        return false;
      if (!(status & ATA_SR_BSY))
        break;
    }

    Asm::sti();
    return true;
  }

  // Kept as thin aliases so any existing callers (and the test_sector_*
  // helpers below) automatically pick up the interrupt-driven path.
  bool read_sector(u32 lba, u8 *buffer) { return read_sector_irq(lba, buffer); }
  bool write_sector(u32 lba, const u8 *buffer) {
    return write_sector_irq(lba, buffer);
  }

  bool test_sector_zero() {
    u8 write_buffer[BLOCK_SIZE] = {};

    write_buffer[0] = 75;
    write_buffer[1] = 67;
    write_buffer[2] = 79;
    write_buffer[3] = 82;

    Debugger::log("=== SECTOR 0 TEST ===\n");

    Debugger::log("BEFORE WRITE: ");
    Debugger::log_number(write_buffer[0]);
    Debugger::log(" ");
    Debugger::log_number(write_buffer[1]);
    Debugger::log(" ");
    Debugger::log_number(write_buffer[2]);
    Debugger::log(" ");
    Debugger::log_number(write_buffer[3]);
    Debugger::log("\n");

    bool write_ok = write_sector(0, write_buffer);

    Debugger::log("WRITE RESULT: ");
    Debugger::log_number(write_ok);
    Debugger::log("\n");

    u8 read_buffer[BLOCK_SIZE] = {};

    bool read_ok = read_sector(0, read_buffer);

    Debugger::log("READ RESULT: ");
    Debugger::log_number(read_ok);
    Debugger::log("\n");

    Debugger::log("AFTER READ: ");
    Debugger::log_number(read_buffer[0]);
    Debugger::log(" ");
    Debugger::log_number(read_buffer[1]);
    Debugger::log(" ");
    Debugger::log_number(read_buffer[2]);
    Debugger::log(" ");
    Debugger::log_number(read_buffer[3]);
    Debugger::log("\n");

    return write_ok && read_ok && read_buffer[0] == 75 &&
           read_buffer[1] == 67 && read_buffer[2] == 79 &&
           read_buffer[3] == 82;
  }

  bool test_sector_one() {
    u8 write_buffer[BLOCK_SIZE] = {};

    write_buffer[0] = 75;
    write_buffer[1] = 67;
    write_buffer[2] = 79;
    write_buffer[3] = 82;

    Debugger::log("=== SECTOR 1 TEST ===\n");
    Debugger::log("WRITE SECTOR 1\n");

    if (!write_sector(1, write_buffer)) {
      Debugger::log("SECTOR 1 WRITE FAILED\n");
      return false;
    }

    Debugger::log("READ SECTOR 1\n");

    u8 read_buffer[BLOCK_SIZE] = {};

    if (!read_sector(1, read_buffer)) {
      Debugger::log("SECTOR 1 READ FAILED\n");
      return false;
    }

    Debugger::log("SECTOR 1 RESULT: ");
    Debugger::log_number(read_buffer[0]);
    Debugger::log(" ");
    Debugger::log_number(read_buffer[1]);
    Debugger::log(" ");
    Debugger::log_number(read_buffer[2]);
    Debugger::log(" ");
    Debugger::log_number(read_buffer[3]);
    Debugger::log("\n");

    return read_buffer[0] == 75 && read_buffer[1] == 67 &&
           read_buffer[2] == 79 && read_buffer[3] == 82;
  }

  bool test_sector_seven() {
    u8 write_buffer[BLOCK_SIZE] = {};

    write_buffer[0] = 75;
    write_buffer[1] = 67;
    write_buffer[2] = 79;
    write_buffer[3] = 82;

    Debugger::log("=== SECTOR 7 TEST ===\n");
    Debugger::log("WRITE SECTOR 7\n");

    if (!write_sector(7, write_buffer)) {
      Debugger::log("SECTOR 7 WRITE FAILED\n");
      return false;
    }

    Debugger::log("READ SECTOR 7\n");

    u8 read_buffer[BLOCK_SIZE] = {};

    if (!read_sector(7, read_buffer)) {
      Debugger::log("SECTOR 7 READ FAILED\n");
      return false;
    }

    Debugger::log("SECTOR 7 RESULT: ");
    Debugger::log_number(read_buffer[0]);
    Debugger::log(" ");
    Debugger::log_number(read_buffer[1]);
    Debugger::log(" ");
    Debugger::log_number(read_buffer[2]);
    Debugger::log(" ");
    Debugger::log_number(read_buffer[3]);
    Debugger::log("\n");

    return read_buffer[0] == 75 && read_buffer[1] == 67 &&
           read_buffer[2] == 79 && read_buffer[3] == 82;
  }
};

// The C++ -> C bridge the assembly stub calls. Kept as a free function
// (not a method) because the CPU jumps to a raw address on IRQ14 - it
// has no idea what a "this" pointer is. We just forward to whichever
// Disk instance registered itself as `active`.
extern "C" void c_disk_handler() {
  if (Disk::active)
    Disk::active->handle_irq();
}
