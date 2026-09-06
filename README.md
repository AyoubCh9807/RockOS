# 🪨 Rock OS

> **A 64-bit operating system built from scratch in C++ and x86 assembly.**

🎸 **Rock. Metal. Code.**

Rock OS is my hobby operating system project, built from scratch to understand what actually happens underneath the software we use every day.

No massive team. No existing desktop environment. No prebuilt kernel.

Just me, C++, assembly, a lot of debugging, and an unreasonable amount of rock music.

**LET'S MAKE A FUCKING OS.**

## What is Rock OS?

Rock OS started as a small kernel experiment.

It has grown into a 64-bit operating system with:

- 🖥️ A graphical desktop
- 🪟 A custom window manager
- 🎨 Framebuffer graphics
- ⌨️ Keyboard input
- 🖱️ Mouse input
- 📋 A taskbar
- 🖼️ Custom wallpapers
- 💬 Dialog boxes
- 💻 A custom terminal and shell
- 💾 ATA PIO storage
- 🧠 Physical memory management
- 📄 Experimental filesystem infrastructure
- ⚙️ Process and context-switching infrastructure
- 🎲 Native GUI applications
- 🎨 A small Paint application
- 📀 A bouncing DVD-style application
- 👑 Experimental applications and utilities

And I'm still building.

**ONE MONTH IN.**

That's the part I'm proud of.

## The idea

I wanted to understand computers at a much lower level.

When a normal application says:

```cpp
printf("hello");
```

there are layers upon layers underneath it.

Rock OS is an attempt to peel those layers back.

I get to ask questions like:

**How does the CPU actually enter the kernel?**

**How does a virtual address become physical memory?**

**How does a timer interrupt happen?**

**How does a process stop and another one start?**

**How does a pixel actually reach the screen?**

**How does a mouse movement become an event inside a window?**

Then I build the answer.

**THAT'S THE FUN PART.**

## 🧠 Kernel

Rock OS is written primarily in **C++**, with x86 assembly used wherever direct CPU interaction is required.

The kernel currently includes infrastructure for:

- Multiboot2 booting
- x86-64 execution
- Interrupt handling
- Timer interrupts
- Keyboard input
- PS/2 mouse input
- Port I/O
- Physical frame allocation
- Kernel heap allocation
- x86-64 paging
- Process structures
- CPU context storage
- Context-switching infrastructure
- ATA PIO disk access
- Filesystem experiments

The project is intentionally growing one subsystem at a time.

**SMALL PIECES. BIG SYSTEM.**

## ⚙️ Processes

Rock OS already has process infrastructure.

A process contains things such as:

- Process ID
- Process state
- Page table
- CPU context
- Instruction pointer
- Stack pointer
- Process stack
- Allocated pages
- Entry point
- Process size

The CPU context tracks registers including:

```text
RAX RBX RCX RDX
RSI RDI RBP
R8  R9  R10 R11
R12 R13 R14 R15
RIP RSP RFLAGS
```

The scheduler and timer interrupt infrastructure are also in place.

**BUT I KNOW WHEN TO STOP.**

I'm currently focusing heavily on the desktop and user-facing side of Rock OS instead of forcing the process system into everything immediately.

I've been building Rock OS for about a month.

There is a lot left to learn.

Keeping the project fun matters.

The process infrastructure is there.

I'll keep expanding it when the rest of the OS is ready for it.

**NO RUSH. KEEP BUILDING.**

## 🧠 Memory & Paging

Rock OS has its own physical frame allocator and kernel heap.

The paging system uses the x86-64 page-table hierarchy:

```text
PML4
 ↓
PDPT
 ↓
Page Directory
 ↓
Page Table
 ↓
4 KiB Page
```

Virtual addresses are broken down into the architecture's page-table indices and translated through these levels.

The kernel also has its own heap for dynamic allocations.

Current work includes:

- Physical frame allocation
- Bitmap-based frame tracking
- Kernel heap
- Page allocation
- Per-process page tables
- Process memory setup

This is one of those parts of OS development where everything looks simple until one wrong address ruins your entire afternoon.

**MEMORY DOES NOT FORGIVE.**

## 🖥️ Graphics

Rock OS eventually escaped the classic VGA text-mode world and moved into direct framebuffer rendering.

The graphics layer provides primitives for:

- Pixels
- Rectangles
- Text
- Colors
- UI elements
- Window surfaces
- Wallpapers

This became the foundation for the graphical desktop.

And honestly?

Seeing something you drew yourself appear on the screen after spending hours staring at low-level code is fucking addictive.

**PIXEL BY PIXEL.**

## 🪟 The Desktop

The desktop is currently one of the biggest parts of Rock OS.

It has:

- Windows
- Window focus
- Window decorations
- Taskbar
- Wallpaper
- Dialog boxes
- Mouse interaction
- Keyboard interaction
- Focus indicators
- Native GUI applications
- Double-buffered rendering

The window manager keeps track of applications and windows while individual applications handle their own behavior.

That means I can build something like a Dice app without rewriting the entire desktop.

**WINDOWS. APPS. CHAOS.**

## 💻 Terminal

Rock OS has its own shell environment too.

Some current commands include:

```text
cat
cd
clear
echo
grep
head
tail
mkdir
rmdir
rm
touch
pwd
basename
dirname
wc
rev
repeat
upper
lower
length
calc
date
uptime
whoami
env
seq
fortune
random
```

There are also custom Rock OS commands and experiments.

For example:

```text
calc 5 + 3 + 7 * 8
```

is parsed and evaluated by the Rock OS command system.

The terminal has its own colored output, cursor handling, scrolling, status information, and command infrastructure.

**THE SHELL STAYS.**

Even with a GUI, I still want the terminal to feel like part of the operating system rather than an afterthought.

## 🎨 Native Apps

Rock OS already has several small graphical applications.

### 🎲 Dice

A simple dice application.

Useful for testing windows, rendering, application updates, and interaction.

### 📀 DVD

A bouncing "Rock OS!" style application.

Simple.

Stupid.

Beautiful.

**IT BOUNCES. THAT'S ALL THAT MATTERS.**

### 🎨 Paint

A small pixel-based drawing application with movement and canvas manipulation.

This one is particularly useful for testing the graphics and application layers.

### 👑 Tyrant

An experimental application built around generated phrases.

Rock OS needs a little insanity too.

## 💾 Storage

Rock OS has an ATA PIO disk driver and an experimental filesystem layer.

The filesystem work includes concepts such as:

- Sectors
- Blocks
- Inodes
- Directories
- File metadata
- File allocation

The filesystem is still evolving.

**DISKS ARE WEIRD.**

And that's exactly why I'm building one.

## 🏗️ Rough Architecture

The current direction looks roughly like this:

```text
┌─────────────────────────────┐
│          Applications      │
│ Dice • Paint • DVD • etc.  │
├─────────────────────────────┤
│       Window Manager        │
│ Windows • Focus • Input    │
├─────────────────────────────┤
│      Graphics / Desktop     │
│ Framebuffer • UI • Text    │
├─────────────────────────────┤
│        Kernel Systems       │
│ Processes • Memory • FS    │
├─────────────────────────────┤
│          Drivers            │
│ Keyboard • Mouse • ATA     │
├─────────────────────────────┤
│          x86-64             │
│ Paging • Interrupts • CPU  │
├─────────────────────────────┤
│       GRUB / Multiboot2     │
└─────────────────────────────┘
```

The boundaries aren't perfect yet.

That's intentional.

Rock OS is still being shaped.

**ARCHITECTURE EVOLVES.**

## 🛠️ Tech Stack

| Part | Technology |
|---|---|
| Main language | C++ |
| Low-level code | x86 Assembly |
| Architecture | x86-64 |
| Bootloader | GRUB |
| Boot protocol | Multiboot2 |
| Virtual machine | QEMU |
| Graphics | Linear framebuffer |
| Input | PS/2 keyboard + mouse |
| Storage | ATA PIO |
| Memory | Physical frames + paging + kernel heap |
| GUI | Custom |
| Window manager | Custom |
| Shell | Custom |
| Filesystem | Custom / experimental |

## 🔥 Why I'm Building It

Because I wanted to stop treating the operating system as a black box.

I wanted to know what happens underneath.

So I started building.

A bootloader gets the CPU into the right environment.

The kernel initializes hardware.

The memory manager starts handing out pages.

Interrupts start firing.

The framebuffer gives me pixels.

The mouse starts moving.

The keyboard starts talking.

Windows appear.

Applications run.

And suddenly there's a tiny computer that I built sitting inside QEMU.

**THAT FEELING NEVER GETS OLD.**

## 🎸 The Name

Rock OS is named after **rock music and metal music**.

That's the vibe.

Heavy music in the headphones.

C++ on the screen.

Kernel panic at 3 AM.

**ROCK ON. 🤘**

## 🚧 Current Status

Rock OS is **actively under development**.

I've been working on it for roughly a month, so plenty of systems are still experimental and some implemented infrastructure isn't fully integrated into the desktop yet.

Current focus is heavily centered around:

- Desktop polish
- Window management
- GUI applications
- Input
- Graphics
- Filesystem development
- Kernel stability
- Expanding the userspace

The goal is to keep making the system more capable without turning development into a miserable grind.

**BUILD. BREAK. FIX. REPEAT.**

## 🗺️ Roadmap

### Kernel

- [ ] Expand process lifecycle management
- [ ] Integrate process infrastructure further into userspace
- [ ] Improve scheduler
- [ ] Improve memory management
- [ ] More hardware support
- [ ] Better kernel diagnostics

### Filesystem

- [ ] Expand filesystem implementation
- [ ] Persistent directories
- [ ] More complete file operations
- [ ] Improved storage abstraction
- [ ] Better filesystem reliability

### Desktop

- [ ] More polished window management
- [ ] Better input routing
- [ ] More desktop utilities
- [ ] More configuration options
- [ ] Improved rendering performance

### Applications

- [ ] More native applications
- [ ] Better application APIs
- [ ] Media support
- [ ] Networking applications
- [ ] More system utilities

### And if this happens...

⭐ **If Rock OS reaches 100 GitHub stars, I'm adding an embedded `.rpp` programming language.**

Yes.

A programming language.

Inside the OS.

**100 STARS. YOU KNOW WHAT TO DO.**

## 🤘 Final Words

Rock OS is a learning project.

It's messy.

It's experimental.

Some parts are tiny.

Some parts are way more complicated than they looked at first.

And there is a ridiculous amount left to build.

That's exactly why I'm having fun with it.

I'm not trying to finish an operating system overnight.

I'm trying to understand computers deeply enough that I can eventually build something genuinely mine.

One subsystem.

One bug.

One pixel.

One crash.

One riff at a time.

# 🪨 ROCK OS

**CODE LOUD.**

**BREAK THINGS.**

**FIX THEM.**

**ROCK ON. 🤘**