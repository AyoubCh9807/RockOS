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

private:
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

public:
  bool read_sector(u32 lba, u8 *buffer) {
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
  bool write_sector(u32 lba, const u8 *buffer) {

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

    for (int i = 0; i < 256; i++) {
      Asm::outw(ATA_IO_BASE + ATA_REG_DATA, source_ptr[i]);
    }

    while (1) {

      u8 status = Asm::inb(ATA_IO_BASE + ATA_REG_STATUS);

      if (status & (ATA_SR_ERR | ATA_SR_DF))
        return false;

      if (!(status & ATA_SR_BSY))
        break;
    }

    /*    Asm::outb(ATA_IO_BASE + ATA_REG_COMMAND, 0xE7);

        while (1) {

          u8 status = Asm::inb(ATA_IO_BASE + ATA_REG_STATUS);

          if (status & (ATA_SR_ERR | ATA_SR_DF))
            return false;

          if (!(status & ATA_SR_BSY))
            break;
        }
    */
    Asm::sti();
    return true;
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
           read_buffer[1] == 67 && read_buffer[2] == 79 && read_buffer[3] == 82;
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
