#pragma once

struct CommandDescription {
  const char *command;
  const char *description;
};

const CommandDescription command_descriptions[] = {

    // =========================
    // FILESYSTEM
    // =========================

    {"ls", "Damian told me once it displays the contents of the current "
           "directory. I think."},

    {"cd", "Move through the filesystem. Tyrant says every path leads "
           "somewhere eventually."},

    {"pwd",
     "Ask RockOS where the hell you are. The machine actually keeps track."},

    {"mkdir", "Forge a new directory from nothing. Damian calls it "
              "construction. I call it another fortress."},

    {"rmdir", "Tear down an empty directory. Tyrant doesn't believe in leaving "
              "abandoned fortresses."},

    {"touch", "Forge a new file on the disk. No contents. No excuses. Just an "
              "empty artifact waiting for a riff."},

    {"rm", "Destroy a file permanently. Damian recommends thinking twice. "
           "Tyrant recommends thinking never."},

    {"cat", "Read the contents of a file straight from the disk. Apparently "
            "the filesystem has stories to tell."},

    // =========================
    // TERMINAL
    // =========================

    {"echo", "Throw words into the machine and listen for the terminal to spit "
             "them back."},

    {"clear", "Wipe the stage clean. Whatever happened before is now buried "
              "beneath the static."},

    {"help", "Ask the machine what it can do. Damian insists you should have "
             "figured it out already."},

    {"uptime", "Find out how long RockOS has survived without being reduced to "
               "a smoking crater."},

    {"reboot",
     "Kill the current session and bring the machine back from the darkness."},

    // =========================
    // CHARACTERS
    // =========================

    {"damian", "Call Damian. He may answer. He may judge your command choices. "
               "Probably both."},

    {"tyrant", "Enter Tyrant's domain. Command carefully. He has very little "
               "patience for weak input."},

    // =========================
    // SYSTEM / IDENTITY
    // =========================

    {"whoami", "Ask RockOS who is sitting at the keyboard. The machine knows "
               "more than you think."},

    {"rockfetch", "Expose the guts of RockOS: kernel, architecture, memory, "
                  "filesystem, and other pieces of the machine."},

    {"stats", "Display the numbers behind the machine. Power, memory, uptime, "
              "and other glorious statistics."},

    {"diagnose", "Tell RockOS to inspect itself. If something is broken, this "
                 "is where the machine starts pointing fingers."},

    // =========================
    // PERSONALITY / RANDOM
    // =========================

    {"fortune", "Pull a random message from the depths of RockOS. Some wisdom. "
                "Some nonsense. Mostly metal."},

    {"motd", "Summon a random message from the machine's front door. RockOS "
             "always has something to say."},

    {"mood", "Ask the machine how it's feeling. The answer may depend on how "
             "badly you've abused the kernel."},

    {"ascii", "Pull a random piece of terminal art from the vault. Because a "
              "shell without ASCII has no soul."},

    {"lore", "Open a random fragment of RockOS history. Some truths are better "
             "left buried in the disk."},

    // =========================
    // VOID
    // =========================

    {"void", "Enter the Void. Damian and Tyrant are currently known. The "
             "remaining silhouettes refuse to reveal themselves."},
};

constexpr int COMMAND_DESCRIPTION_COUNT =
    sizeof(command_descriptions) / sizeof(command_descriptions[0]);
