#pragma once

const char* boot_phrases[] = {
    "RockOS is awake.\n",
    "Welcome to the noise.\n",
    "Kernel loaded. Volume: MAXIMUM.\n",
    "The machine lives.\n",
    "Boot sequence complete. Let the riff begin.\n",
    "System online. Damian is watching.\n",
    "Tyrant has entered the system.\n",
    "Welcome back, rocker.\n",
    "RockOS has risen from the static.\n",
    "The forge is burning.\n",
    "The machine remembers.\n",
    "RockOS // Systems nominal. Chaos permitted.\n",
    "The kernel survived another boot.\n",
    "Static cleared. Machine ready.\n",
    "The beast has awakened.\n",
};

constexpr int BOOT_PHRASE_COUNT =
    sizeof(boot_phrases) / sizeof(boot_phrases[0]);
