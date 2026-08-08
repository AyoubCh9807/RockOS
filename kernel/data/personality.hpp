#pragma once

const char* mood_names[] = {
    "CALM",
    "AGGRESSIVE",
    "EXCITED",
    "CHAOTIC",
    "TIRED",
    "FOCUSED",
    "DOMINANT",
    "RESTLESS",
};

const char* mood_descriptions[] = {
    "The machine is quiet. For now.\n",
    "The machine wants something broken.\n",
    "The machine is running hot.\n",
    "Logic has temporarily left the building.\n",
    "Even the kernel needs a moment.\n",
    "RockOS is locked in.\n",
    "Tyrant is pleased.\n",
    "The machine refuses to sit still.\n",
};

const char* mood_damian_reactions[] = {
    "Damian: The forge is calm.\n",
    "Damian: Everything is under control.\n",
    "Damian: Let's make some noise.\n",
    "Damian: Now we're getting somewhere.\n",
    "Damian: The machine is getting restless.\n",
    "Damian: Focus. We've got work to do.\n",
    "Damian: Someone woke Tyrant up.\n",
    "Damian: I can feel the machine humming.\n",
};

const char* mood_tyrant_reactions[] = {
    "Tyrant: Silence.\n",
    "Tyrant: The machine demands strength.\n",
    "Tyrant: Proceed with purpose.\n",
    "Tyrant: Chaos detected. Acceptable.\n",
    "Tyrant: Even the machine must rest.\n",
    "Tyrant: Focus.\n",
    "Tyrant: Power is stable.\n",
    "Tyrant: Something is coming.\n",
};

constexpr int MOOD_COUNT =
    sizeof(mood_names) / sizeof(mood_names[0]);

constexpr int MOOD_DESCRIPTION_COUNT =
    sizeof(mood_descriptions) / sizeof(mood_descriptions[0]);

constexpr int MOOD_DAMIAN_REACTION_COUNT =
    sizeof(mood_damian_reactions) /
    sizeof(mood_damian_reactions[0]);

constexpr int MOOD_TYRANT_REACTION_COUNT =
    sizeof(mood_tyrant_reactions) /
    sizeof(mood_tyrant_reactions[0]);


/*
 * VOID CHARACTER ROSTER
 *
 * 0 = Damian
 * 1 = Tyrant
 * 2-6 = hidden
 */

const char* void_character_names[] = {
    "Damian",
    "Tyrant",
    "???",
    "???",
    "???",
    "???",
    "???",
};

const char* void_character_titles[] = {
    "The Builder",
    "The Tyrant",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
};

const bool void_character_unlocked[] = {
    true,
    true,
    false,
    false,
    false,
    false,
    false,
};

const char* void_character_status[] = {
    "UNLOCKED",
    "UNLOCKED",
    "HIDDEN",
    "HIDDEN",
    "HIDDEN",
    "HIDDEN",
    "HIDDEN",
};

constexpr int VOID_CHARACTER_COUNT =
    sizeof(void_character_names) /
    sizeof(void_character_names[0]);

constexpr int VOID_UNLOCKED_CHARACTER_COUNT = 2;

constexpr int VOID_HIDDEN_CHARACTER_COUNT =
    VOID_CHARACTER_COUNT - VOID_UNLOCKED_CHARACTER_COUNT;
