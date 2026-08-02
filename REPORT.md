# REPORT.md — Debugging the Rock OS Disk Driver

## Summary

`FileSystem::format()` appeared to freeze the entire OS. Debugging this
surfaced **three separate, independent bugs** in the ATA (IDE) disk driver
(`kernel/storage/disk.hpp`) and the filesystem layer
(`kernel/storage/file_system.hpp`). None of them were in `format()` itself —
that function was always correct or trivial. All three bugs were found by
isolating the problem step by step: first proving the hang lived inside
`disk.write_sector`, then fixing port addressing (twice), then proving via
raw hex dumps that writes were silently failing at the hardware level, which
led to the real root cause in the 16-bit I/O helpers.

---

## Bug #1 — Inconsistent ATA port offsets caused an infinite busy-wait hang

### Symptom
Calling `fs.format()` froze the OS completely: nothing printed, no crash,
no reboot — just a dead system.

### Root cause
The ATA register constants were defined inconsistently, and the code that
used them didn't apply a consistent base offset. At different points in the
debugging process the constants looked like this:

```cpp
// First broken version
constexpr static u16 ATA_REG_STATUS  = 0x1F7;  // already a FULL port address
constexpr static u16 ATA_REG_COMMAND = 0x1F7;  // already a FULL port address
constexpr static u16 ATA_REG_SECCOUNT = 0x02;  // an OFFSET from 0x1F0
constexpr static u16 ATA_REG_LBA_LOW  = 0x03;  // an OFFSET
...
```

while `write_sector` called them like this:

```cpp
while (Asm::inb(ATA_REG_STATUS) & ATA_SR_BSY)   // ❌ no base offset added
  ;
...
Asm::outb(0x1F0 + ATA_REG_DEVICE, ...);          // ✅ offset added correctly
...
Asm::outb(ATA_REG_COMMAND, 0x30);                // ❌ no base offset added
```

Because `ATA_REG_STATUS`/`ATA_REG_COMMAND` were sometimes bare offsets
(`0x07`) and sometimes full addresses (`0x1F7`), and the call sites
sometimes added `0x1F0 +` and sometimes didn't, the busy-wait loop:

```cpp
while (Asm::inb(ATA_REG_STATUS) & ATA_SR_BSY)
  ;
```

ended up reading a completely unrelated I/O port (e.g. `0x0007`, or
`0x1F0 + 0x1F7 = 0x2E7`) instead of the real ATA status register at
`0x1F7`. Whatever garbage byte came back from that wrong port happened to
have the busy bit (`0x80`) permanently set, so the loop spun forever before
a single real ATA command was ever sent to the drive. This is exactly why
nothing printed: the freeze happened on the very first disk access, before
any later code (including `Terminal::print` calls that came after it) had a
chance to run.

### Fix
Standardized on **one convention everywhere**: every ATA register constant
is a small offset from a single base, and every single access — with zero
exceptions — adds that base explicitly:

```cpp
constexpr static u16 ATA_IO_BASE = 0x1F0;
constexpr static u16 ATA_REG_DATA     = 0x00;
constexpr static u16 ATA_REG_SECCOUNT = 0x02;
constexpr static u16 ATA_REG_LBA_LOW  = 0x03;
constexpr static u16 ATA_REG_LBA_MID  = 0x04;
constexpr static u16 ATA_REG_LBA_HIGH = 0x05;
constexpr static u16 ATA_REG_DEVICE   = 0x06;
constexpr static u16 ATA_REG_STATUS   = 0x07;
constexpr static u16 ATA_REG_COMMAND  = 0x07;
```

And every access became `Asm::inb(ATA_IO_BASE + ATA_REG_X)` /
`Asm::outb(ATA_IO_BASE + ATA_REG_X, ...)`, in `read_sector`, `write_sector`,
and `poll()` alike. This eliminated the hang.

---

## Bug #2 — `format()` never initialized the directory sectors

### Symptom
After Bug #1 was fixed, the OS booted fine and printed all expected
checkpoints, but `write_file("hello.txt", ...)` followed by
`read_file("hello.txt", ...)` produced an empty buffer — nothing was ever
found or printed.

### Root cause
`format()` only zeroed and wrote the **superblock** (sector 0). It never
touched the directory sectors (`DIR_LBA_START` .. `MAX_LBA`, i.e. sectors
6–10), which is where `FileEntry` structs live:

```cpp
void format() {
  u8 sector_buffer[512];
  for (int i = 0; i < 512; i++)
    sector_buffer[i] = 0;

  u32 *magic_ptr = (u32 *)sector_buffer;
  *magic_ptr = 0x524F434B; // "ROCK"
  disk.write_sector(0, sector_buffer);
  // <-- returns here; directory sectors never touched
}
```

`write_file`'s free-slot search relies entirely on `is_used` being `false`
in a fresh, never-written `FileEntry`:

```cpp
for (int i = 0; i < FILES_PER_BLOCK; i++) {
  if (!file_entries[i].is_used) {   // reads raw, unformatted disk bytes
    ...
  }
}
```

Since those sectors were never explicitly zeroed by the filesystem, this
depended entirely on whatever bytes happened to already be on the disk
image. Whether or not this "worked" was pure luck based on how `disk.img`
was created — it was never guaranteed by the code.

### Fix
`format()` now zero-writes every directory sector too, guaranteeing every
`FileEntry.is_used` starts `false`:

```cpp
void format() {
  u8 sector_buffer[512];
  for (int i = 0; i < 512; i++)
    sector_buffer[i] = 0;

  u32 *magic_ptr = (u32 *)sector_buffer;
  *magic_ptr = 0x524F434B; // "ROCK"
  disk.write_sector(0, sector_buffer);

  *magic_ptr = 0; // don't leak the magic number into directory sectors

  for (u32 dir_sector = DIR_LBA_START; dir_sector < MAX_LBA; dir_sector++) {
    disk.write_sector(dir_sector, sector_buffer);
  }
}
```

---

## Bug #3 (root cause) — `inw`/`outw` used two 8-bit accesses instead of one 16-bit access

### Symptom
After Bugs #1 and #2 were fixed, the OS booted cleanly, every checkpoint
printed, and raw ATA status diagnostics showed a **perfectly healthy
handshake** (status `0x58` = DRDY + DSC + DRQ, no error bits, no busy bit).
Despite that, every sector read back from disk — sector 0 (superblock),
sector 6 (directory), sector 1 (file data) — was still all zeros. The
protocol sequencing (drive select, LBA setup, command, DRQ wait) was
completely correct, yet no data was ever actually landing on disk.

### Root cause
The 16-bit port I/O helpers in `asm.hpp` were fakes — they issued two
separate 8-bit instructions instead of one real 16-bit instruction:

```cpp
static u16 inw(u16 port) {
  u8 low = inb(port);
  u8 high = inb(port);
  return (static_cast<u16>(high) << 8) | low;
}

static void outw(u16 port, u16 value) {
  outb(port, (u8)(value & 0xFF));
  outb(port, (u8)(value >> 8));
}
```

In assembly this compiles to two `out %al, %dx` (byte-sized) instructions,
never a single `out %ax, %dx` (word-sized) instruction. The ATA data
register (`0x1F0`) is specifically required by the ATA PIO protocol to be
accessed with genuine 16-bit-wide I/O cycles — this is how the controller's
internal buffer/latch advances one word at a time. Two separate 8-bit
accesses to the same port do not behave the same way on real or emulated
IDE hardware; QEMU's IDE controller simply didn't advance its internal
state the way a real word access would, so none of the 256 words per
sector transfer actually reached the drive's buffer — with no error
reported anywhere, since from the controller's point of view nothing
"wrong" happened, it just never received the expected word-sized writes.

This explains every earlier observation:
- Clean status/handshake bits (the *command* protocol was fine).
- No hang, no error bit (nothing failed in a way ATA reports as an error).
- All-zero sectors on every read-back (the actual 512-byte payload never
  transferred).

### Fix
Replaced the fake helpers with real word-sized `in`/`out` instructions:

```cpp
static inline u16 inw(u16 port) {
  u16 result;
  __asm__ volatile("inw %1, %0" : "=a"(result) : "Nd"(port));
  return result;
}

static inline void outw(u16 port, u16 value) {
  __asm__ volatile("outw %0, %1" : : "a"(value), "Nd"(port));
}
```

No other code needed to change — `read_sector`/`write_sector` already
called `Asm::inw`/`Asm::outw` correctly; only the implementation of those
two helpers was wrong. After this fix, `xxd`/in-kernel hex dumps confirmed
real data (`48 65 6C 6C 6F ...` = `"Hello..."`) landing correctly in the
target sectors.

---

## How each bug was actually diagnosed

1. **Isolation by comment-out.** Confirmed the freeze was specifically
   inside `disk.write_sector`, not `format()`'s buffer logic, by
   temporarily stripping `format()` down to a single `write_sector` call.
2. **Port constant audit.** Manually cross-referenced every constant
   definition against every call site to find the inconsistent offset
   usage (Bug #1), fixed, rebuilt, retested.
3. **Boot-order breadcrumbs.** Once the hang was gone but output was still
   wrong, added sequential `Terminal::print("Step N...")` calls through
   `kernel_main` to find exactly how far execution got before something
   silently failed (revealed everything ran to completion, ruling out a
   crash or second hang).
4. **In-kernel hex dumps.** Read sectors back immediately after writing
   them and printed their raw bytes to the screen, rather than trusting
   `xxd` on the host disk image (which added uncertainty about whether
   QEMU had actually flushed writes to the host file, and whether the
   guest OS had even booted far enough / been shut down cleanly before the
   check). This proved the *writes themselves* were failing, not a
   host-side caching artifact (Bug #2, then Bug #3).
5. **Raw ATA protocol probe.** Bypassed the `Disk` class entirely and
   issued a manual `WRITE SECTORS` command directly from `kernel_main`,
   printing the status/error registers after each poll iteration. This
   proved the command handshake itself was completely healthy, narrowing
   the remaining bug down to the data-transfer phase specifically — which
   led directly to finding the fake `inw`/`outw` implementation.

---

## Files changed

- `kernel/storage/disk.hpp` — consistent `ATA_IO_BASE + offset` addressing
  on every register access (Bug #1).
- `kernel/storage/file_system.hpp` — `format()` now zeroes all directory
  sectors, not just the superblock (Bug #2).
- `kernel/core/asm.hpp` — `inw`/`outw` now use real word-sized `in`/`out`
  assembly instructions instead of two chained byte accesses (Bug #3, the
  actual root cause of the missing data).

## Known follow-ups (not yet fixed, tracked for later)

- `write_sector`/`write_file` always transfer a full 512-byte sector
  regardless of the caller's `size` argument, which can read past the end
  of a short buffer (e.g. a `kmalloc`'d string shorter than 512 bytes).
  Harmless today because `Terminal::print` stops at the first `\0`, but
  should be fixed by zero-padding a staging buffer before writing.
- `write_file` has no path support, no directory hierarchy, no multi-sector
  files, and no free-space tracking — it addresses blocks by a fixed
  arithmetic formula tied to directory-sector position. This is the
  planned next phase of work (path resolution, block allocator bitmap,
  multi-block file chains).
- `write_file` does not check whether a file with the given name already
  exists before writing a new entry — calling it twice with the same name
  does not overwrite the original.
