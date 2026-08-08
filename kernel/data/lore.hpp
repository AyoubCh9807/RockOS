#pragma once

const char* damian_lore[] = {
    "Damian is the builder behind the machine.\n",
    "Damian speaks through RockOS when the machine needs a voice.\n",
    "Damian believes broken systems are simply unfinished systems.\n",
    "Damian built the forge. The operator keeps it burning.\n",
    "Damian does not fear crashes. He learns from them.\n",
    "Damian watches the filesystem like a craftsman watches his tools.\n",
    "Damian's rule is simple: build it, break it, understand it.\n",
    "Damian is the voice of creation inside RockOS.\n",
};

const char* tyrant_lore[] = {
    "Tyrant is the authority of RockOS.\n",
    "Tyrant does not ask the machine to obey. Tyrant expects it.\n",
    "Tyrant watches the filesystem from the depths of the kernel.\n",
    "Tyrant represents control, power and absolute order.\n",
    "Tyrant tolerates chaos. Tyrant does not tolerate corruption.\n",
    "Tyrant commands. RockOS responds.\n",
    "Tyrant has never accepted a weak command.\n",
    "Nobody knows whether Tyrant protects the machine or owns it.\n",
};

const char* rockos_lore[] = {
    "RockOS was built from code, hardware and unreasonable ambition.\n",
    "RockOS exists to turn a machine into something that feels alive.\n",
    "The machine is young. The machine is learning.\n",
    "RockOS does not pretend to be Unix. It has its own voice.\n",
    "Every subsystem added to RockOS becomes another piece of its identity.\n",
    "The kernel is the heart. The shell is the voice. The filesystem is the memory.\n",
    "RockOS was born in the forge and raised in the debugger.\n",
    "The machine survives one bug at a time.\n",
};

const char* rockfs_lore[] = {
    "RockFS is the memory of the machine.\n",
    "Every inode describes something the filesystem remembers.\n",
    "Every block belongs somewhere.\n",
    "Directories connect the pieces of the filesystem together.\n",
    "The root directory is where RockFS begins.\n",
    "Bitmaps tell RockFS what remains available.\n",
    "A filesystem is only as trustworthy as its bookkeeping.\n",
    "RockFS is young, but it remembers.\n",
};

const char* kernel_lore[] = {
    "The kernel is where RockOS becomes more than a program.\n",
    "Interrupts give the machine a voice.\n",
    "Memory gives the machine somewhere to work.\n",
    "The disk gives the machine somewhere to remember.\n",
    "The scheduler will eventually teach the machine how to multitask.\n",
    "Every driver is another conversation between RockOS and hardware.\n",
    "The kernel is small. The ambition is not.\n",
};

constexpr int DAMIAN_LORE_COUNT =
    sizeof(damian_lore) / sizeof(damian_lore[0]);

constexpr int TYRANT_LORE_COUNT =
    sizeof(tyrant_lore) / sizeof(tyrant_lore[0]);

constexpr int ROCKOS_LORE_COUNT =
    sizeof(rockos_lore) / sizeof(rockos_lore[0]);

constexpr int ROCKFS_LORE_COUNT =
    sizeof(rockfs_lore) / sizeof(rockfs_lore[0]);

constexpr int KERNEL_LORE_COUNT =
    sizeof(kernel_lore) / sizeof(kernel_lore[0]);
