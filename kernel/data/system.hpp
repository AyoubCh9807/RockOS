#pragma once

const char* whoami_phrases[] = {
    "You are the operator.\n",
    "You are root. For now.\n",
    "You are the one behind the keyboard.\n",
    "You are the architect of this machine.\n",
    "You are the voice typing into the void.\n",
    "You are the operator. RockOS is listening.\n",
    "You are root. Try not to destroy everything.\n",
    "You are the human component of the system.\n",
    "You are here. The machine knows it.\n",
    "You are the one giving the commands.\n",
    "Damian calls you the operator.\n",
    "Tyrant calls you subordinate.\n",
};

constexpr int WHOAMI_PHRASE_COUNT =
    sizeof(whoami_phrases) / sizeof(whoami_phrases[0]);


const char* motd_messages[] = {
    "ROCKOS // WELCOME BACK\n"
    "The machine is awake.\n"
    "The forge is burning.\n"
    "Let's make some noise.\n",

    "ROCKOS // SYSTEM ONLINE\n"
    "Damian is watching.\n"
    "Tyrant is waiting.\n"
    "The filesystem stands.\n",

    "ROCKOS // ANOTHER DAY\n"
    "Another boot.\n"
    "Another kernel.\n"
    "Another opportunity to break something.\n",

    "ROCKOS // THE MACHINE LIVES\n"
    "Memory initialized.\n"
    "Filesystem mounted.\n"
    "Shell ready.\n"
    "Volume: MAXIMUM.\n",

    "ROCKOS // WELCOME, OPERATOR\n"
    "Know where you're going.\n"
    "Know what you're deleting.\n"
    "And never trust a bad pointer.\n",

    "ROCKOS // FROM THE FORGE\n"
    "Iron.\n"
    "Code.\n"
    "Static.\n"
    "Noise.\n"
    "RockOS.\n",

    "ROCKOS // TYRANT'S DOMAIN\n"
    "The machine obeys.\n"
    "The filesystem stands.\n"
    "Proceed.\n",

    "ROCKOS // DAMIAN'S NOTE\n"
    "Keep building.\n"
    "Keep breaking.\n"
    "Keep learning.\n"
    "Make something worth remembering.\n",
};

constexpr int MOTD_MESSAGE_COUNT =
    sizeof(motd_messages) / sizeof(motd_messages[0]);


/*
 * Hardcoded rockfetch data.
 * Replace the values later with real kernel/runtime values.
 */

const char* rockfetch_labels[] = {
    "OS",
    "Kernel",
    "Architecture",
    "Shell",
    "Filesystem",
    "CPU",
    "Memory",
    "Heap",
    "Disk",
    "Character",
    "Status",
    "Noise",
};

const char* rockfetch_values[] = {
    "RockOS",
    "0.1",
    "x86",
    "RockShell",
    "RockFS",
    "x86 CPU",
    "16 MB",
    "4 MB",
    "ATA",
    "Tyrant",
    "ALIVE",
    "MAXIMUM",
};

constexpr int ROCKFETCH_FIELD_COUNT =
    sizeof(rockfetch_labels) / sizeof(rockfetch_labels[0]);


/*
 * Hardcoded stats.
 * Replace these with real values later.
 */

const char* stat_labels[] = {
    "Commands executed",
    "Successful commands",
    "Failed commands",
    "Files created",
    "Files destroyed",
    "Directories created",
    "Directories destroyed",
    "Bytes written",
    "Bytes read",
    "Blocks allocated",
    "Blocks freed",
    "Inodes allocated",
    "Inodes freed",
    "Shell sessions",
    "Reboots",
    "Kernel panics",
};

const char* stat_values[] = {
    "143",
    "131",
    "12",
    "24",
    "7",
    "9",
    "2",
    "8192",
    "16384",
    "41",
    "17",
    "31",
    "8",
    "6",
    "4",
    "0",
};

constexpr int STAT_FIELD_COUNT =
    sizeof(stat_labels) / sizeof(stat_labels[0]);


/*
 * Hardcoded diagnostic data.
 * Replace the values later with actual subsystem checks.
 */

const char* diagnostic_labels[] = {
    "CPU",
    "Memory",
    "Heap",
    "ATA",
    "Interrupts",
    "Keyboard",
    "Timer",
    "Filesystem",
    "Superblock",
    "Inode bitmap",
    "Block bitmap",
    "Inode table",
    "Root inode",
    "Root directory",
    "Shell",
};

const char* diagnostic_values[] = {
    "ONLINE",
    "OK",
    "OK",
    "ONLINE",
    "ENABLED",
    "ONLINE",
    "ONLINE",
    "MOUNTED",
    "VALID",
    "VALID",
    "VALID",
    "VALID",
    "VALID",
    "VALID",
    "READY",
};

constexpr int DIAGNOSTIC_FIELD_COUNT =
    sizeof(diagnostic_labels) / sizeof(diagnostic_labels[0]);
