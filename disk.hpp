#include "asm.hpp"
#include "shared/types.hpp"

class Disk {

  constexpr static u8 ATA_SR_BSY = 0x80; // Bit 7: Busy
  constexpr static u8 ATA_SR_DRQ = 0x08; // Bit 3: Data Request
  constexpr static u8 ATA_SR_ERR = 0x01; // Bit 0: Error
  constexpr static u16 ATA_REG_STATUS = 0x1F7;
  constexpr static u16 ATA_REG_DATA = 0x1F0;
  constexpr static u16 ATA_REG_SECCOUNT = 0x02; // Offset from 0x1F0 -> 0x1F2
  constexpr static u16 ATA_REG_LBA_LOW = 0x03;  // 0x1F3
  constexpr static u16 ATA_REG_LBA_MID = 0x04;  // 0x1F4
  constexpr static u16 ATA_REG_LBA_HIGH = 0x05; // 0x1F5
  constexpr static u16 ATA_REG_DEVICE = 0x06;   // 0x1F6
  constexpr static u16 ATA_REG_COMMAND = 0x1F7;

private:
  bool is_ready = false;

  void poll() {
    while (1) {
      u8 status = Asm::inb(ATA_REG_STATUS);

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
    // Wait until the drive is not busy before sending parameters
    while (Asm::inb(ATA_REG_STATUS) & ATA_SR_BSY)
      ;

    // Send drive selection and LBA highest bits (using 28-bit LBA mode)
    // 0xE0 means: Master drive (0xA0) + LBA mode bit enabled (0x10) + top 4
    // bits of LBA
    Asm::outb(0x1F0 + ATA_REG_DEVICE, 0xE0 | ((lba >> 24) & 0x0F));

    // Send sector count (we want just 1 sector for now)
    Asm::outb(0x1F0 + ATA_REG_SECCOUNT, 1);

    // Send the LBA address split across low, mid, and high ports
    Asm::outb(0x1F0 + ATA_REG_LBA_LOW, (u8)lba);
    Asm::outb(0x1F0 + ATA_REG_LBA_MID, (u8)(lba >> 8));
    Asm::outb(0x1F0 + ATA_REG_LBA_HIGH, (u8)(lba >> 16));

    // Send the Read Sector command (0x20) to the command register
    Asm::outb(ATA_REG_COMMAND, 0x20);

    // Poll until the drive tells us the sector data is loaded and ready
    poll();

    if (!is_ready) {
      // Handle read failure here
      return;
    }

    // Read the 512-byte sector data 16 bits (2 bytes) at a time
    // A standard sector is 512 bytes, which equals 256 words (u16)
    u16 *target_ptr = (u16 *)buffer;
    for (int i = 0; i < 256; i++) {
      target_ptr[i] = Asm::inw(ATA_REG_DATA);
    }
  }

  void *write_sector(u32 lba, const u8 *buffer) {
    // Wait until the drive is not busy before sending parameters
    while (Asm::inb(ATA_REG_STATUS) & ATA_SR_BSY)
      ;

    // Send drive selection and LBA highest bits (using 28-bit LBA mode)
    // 0xE0 means: Master drive (0xA0) + LBA mode bit enabled (0x10) + top 4
    // bits of LBA
    Asm::outb(0x1F0 + ATA_REG_DEVICE, 0xE0 | ((lba >> 24) & 0x0F));

    // Send sector count (we want just 1 sector for now)
    Asm::outb(0x1F0 + ATA_REG_SECCOUNT, 1);

    // Send the LBA address split across low, mid, and high ports
    Asm::outb(0x1F0 + ATA_REG_LBA_LOW, (u8)lba);
    Asm::outb(0x1F0 + ATA_REG_LBA_MID, (u8)(lba >> 8));
    Asm::outb(0x1F0 + ATA_REG_LBA_HIGH, (u8)(lba >> 16));

    // Send the Write Sector command (0x30) to the command register
    Asm::outb(ATA_REG_COMMAND, 0x30);

    // Poll until the drive tells us the sector data is loaded and ready
    poll();

    if (!is_ready) {
      // Handle read failure here
      return 0;
    }

    const u16 *source_ptr = (const u16 *)buffer;
    for (int i = 0; i < 256; i++) {
      Asm::outw(ATA_REG_DATA, source_ptr[i]);
    }

    Asm::outb(ATA_REG_COMMAND, 0xE7);
    // poll briefly until BSY clears to ensure flush is done
    while (Asm::inb(ATA_REG_STATUS) & ATA_SR_BSY)
      ;
  }
};
