#pragma once

const char* fortune_phrases[] = {
    "\"Every kernel starts with one unreasonable idea.\"",
    "\"The filesystem remembers what the operator forgets.\"",
    "\"A corrupted block is just a bad day written to disk.\"",
    "\"The machine does not fear the reboot. Neither should you.\"",
    "\"Every inode has a story. Every block has a place.\"",
    "\"If it compiles, test it. If it boots, test it harder.\"",
    "\"The kernel is only as strong as its weakest pointer.\"",
    "\"A good operator knows where they are before deleting anything.\"",
    "\"The machine lives between the interrupt and the return.\"",
    "\"Never trust a pointer you didn't earn.\"",
    "\"The filesystem speaks in blocks.\"",
    "\"One day the bug will reveal itself. Until then, debug.\"",
    "\"Hardware does not care about your excuses.\"",
    "\"The stack remembers. The heap forgets.\"",
    "\"Every crash is another lesson written in machine code.\"",
    "\"RockOS does not need permission to rock.\"",
    "\"When in doubt, inspect the bytes.\"",
    "\"The void contains many bugs.\"",
    "\"A clean boot is worth a thousand excuses.\"",
    "\"Do not fear the panic. Understand it.\"",
    "\"Damian builds. Tyrant commands. RockOS survives.\"",
    "\"The riff ends. The debugging doesn't.\"",
    "\"Some bugs hide in the smallest byte.\"",
    "\"The machine rewards patience and punishes assumptions.\"",
    "\"Your filesystem is only as reliable as your bookkeeping.\"",
    "\"Never underestimate one bad sector.\"",
    "\"The kernel knows when you cheated.\"",
    "\"Today you debug. Tomorrow you understand.\"",
    "\"If the machine is silent, check the interrupt.\"",
    "\"The forge burns hottest during a hard debug session.\"",
};

constexpr int FORTUNE_PHRASE_COUNT =
    sizeof(fortune_phrases) / sizeof(fortune_phrases[0]);
