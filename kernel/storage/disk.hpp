#pragma once

#include "../shared/types.hpp"
#include "../core/asm.hpp"

class Disk {
  constexpr static u8 ATA_SR_BSY = 0x80;
  constexpr static u8 ATA_SR_DRQ = 0x08;
  constexpr static u8 ATA_SR_ERR = 0x01;

  constexpr static u16 ATA_IO_BASE = 0x1F0;
  constexpr static u16 ATA_REG_DATA     = 0x00;
  constexpr static u16 ATA_REG_SECCOUNT = 0x02;
  constexpr static u16 ATA_REG_LBA_LOW  = 0x03;
  constexpr static u16 ATA_REG_LBA_MID  = 0x04;
  constexpr static u16 ATA_REG_LBA_HIGH = 0x05;
  constexpr static u16 ATA_REG_DEVICE   = 0x06;
  constexpr static u16 ATA_REG_STATUS   = 0x07;
  constexpr static u16 ATA_REG_COMMAND  = 0x07;

private:
  bool is_ready = false;

  void poll() {
    while (1) {
      u8 status = Asm::inb(ATA_IO_BASE + ATA_REG_STATUS);   // fixed

      if (!(status & ATA_SR_BSY) && (status & ATA_SR_DRQ)) {
        is_ready = true;
        break;
      }
      if (status & ATA_SR_ERR) {
        is_ready = false;
        break;
      }
    }
  }

public:
  void read_sector(u32 lba, u8 *buffer) {
    while (Asm::inb(ATA_IO_BASE + ATA_REG_STATUS) & ATA_SR_BSY)  // fixed
      ;

    Asm::outb(ATA_IO_BASE + ATA_REG_DEVICE, 0xE0 | ((lba >> 24) & 0x0F));
    Asm::outb(ATA_IO_BASE + ATA_REG_SECCOUNT, 1);
    Asm::outb(ATA_IO_BASE + ATA_REG_LBA_LOW, (u8)lba);
    Asm::outb(ATA_IO_BASE + ATA_REG_LBA_MID, (u8)(lba >> 8));
    Asm::outb(ATA_IO_BASE + ATA_REG_LBA_HIGH, (u8)(lba >> 16));
    Asm::outb(ATA_IO_BASE + ATA_REG_COMMAND, 0x20);   // fixed

    poll();
    if (!is_ready) return;

    u16 *target_ptr = (u16 *)buffer;
    for (int i = 0; i < 256; i++) {
      target_ptr[i] = Asm::inw(ATA_IO_BASE + ATA_REG_DATA);   // fixed
    }
  }

  void write_sector(u32 lba, const u8 *buffer) {   // also see note below re: return type
    while (Asm::inb(ATA_IO_BASE + ATA_REG_STATUS) & ATA_SR_BSY)   // fixed
      ;

    Asm::outb(ATA_IO_BASE + ATA_REG_DEVICE, 0xE0 | ((lba >> 24) & 0x0F));
    Asm::outb(ATA_IO_BASE + ATA_REG_SECCOUNT, 1);
    Asm::outb(ATA_IO_BASE + ATA_REG_LBA_LOW, (u8)lba);
    Asm::outb(ATA_IO_BASE + ATA_REG_LBA_MID, (u8)(lba >> 8));
    Asm::outb(ATA_IO_BASE + ATA_REG_LBA_HIGH, (u8)(lba >> 16));
    Asm::outb(ATA_IO_BASE + ATA_REG_COMMAND, 0x30);   // fixed

    poll();
    if (!is_ready) return;

    const u16 *source_ptr = (const u16 *)buffer;
    for (int i = 0; i < 256; i++) {
      Asm::outw(ATA_IO_BASE + ATA_REG_DATA, source_ptr[i]);   // fixed
    }

    Asm::outb(ATA_IO_BASE + ATA_REG_COMMAND, 0xE7);   // fixed
    while (Asm::inb(ATA_IO_BASE + ATA_REG_STATUS) & ATA_SR_BSY)   // fixed
      ;
  }
};
