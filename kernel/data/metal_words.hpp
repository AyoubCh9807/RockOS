#pragma once

const char *metal_word_list[] = {
    "thunder",    "lightning",    "storm",       "DARKNESS",    "shadow",
    "blood",      "bone",         "skull",       "iron",        "steel",
    "metal",      "ROCK",         "heavy",       "doom",        "death",
    "slaughter",  "massacre",     "chaos",       "fury",        "rage",
    "wrath",      "vengeance",    "revenge",     "venom",       "poison",
    "toxic",      "acid",         "nuclear",     "atomic",      "radiation",
    "apocalypse", "armageddon",   "destruction", "ruin",        "wreckage",
    "fallout",    "wasteland",    "desert",      "mountain",    "volcano",
    "magma",      "lava",         "obsidian",    "crystal",     "monolith",
    "citadel",    "fortress",     "castle",      "dungeon",     "prison",
    "chain",      "shackle",      "whip",        "torture",     "pain",
    "agony",      "suffering",    "torment",     "scream",      "cry",
    "howl",       "roar",         "groan",       "whisper",     "echo",
    "silence",    "glitch",       "matrix",      "cyber",       "digital",
    "net",        "web",          "code",        "byte",        "core",
    "kernel",     "terminal",     "system",      "CRASH",       "panic",
    "root",       "boot",         "shell",       "disk",        "drive",
    "sector",     "track",        "buffer",      "stack",       "heap",
    "leak",       "fault",        "interrupt",   "vector",      "thread",
    "process",    "daemon",       "mirror",      "prism",       "lens",
    "focus",      "beam",         "ray",         "laser",       "plasma",
    "spark",      "flash",        "blaze",       "INFERNO",     "pyre",
    "furnace",    "forge",        "anvil",       "hammer",      "nail",
    "spike",      "altar",        "temple",      "shrine",      "cathedral",
    "chapel",     "prayer",       "hymn",        "psalm",       "chant",
    "choir",      "bell",         "toll",        "grief",       "sorrow",
    "woe",        "despair",      "madness",     "insanity",    "lunatic",
    "asylum",     "psycho",       "killer",      "executioner", "hangman",
    "guillotine", "gallows",      "noose",       "trap",        "snare",
    "spider",     "sting",        "bite",        "claw",        "fang",
    "horn",       "tail",         "wing",        "feather",     "talon",
    "beak",       "eye",          "gaze",        "stare",       "blind",
    "sight",      "vision",       "dream",       "nightmare",   "sleep",
    "eternity",   "forever",      "time",        "space",       "cosmos",
    "star",       "sun",          "moon",        "planet",      "galaxy",
    "universe",   "black",        "night",       "midnight",    "dusk",
    "twilight",   "dawn",         "eclipse",     "shade",       "penumbra",
    "aura",       "spirit",       "soul",        "mind",        "brain",
    "thought",    "idea",         "concept",     "theory",      "thesis",
    "hypothesis", "axiom",        "dogma",       "creed",       "faith",
    "belief",     "truth",        "myth",        "legend",      "lore",
    "saga",       "epic",         "tale",        "story",       "book",
    "scroll",     "parchment",    "quill",       "ink",         "pen",
    "writer",     "author",       "poet",        "singer",      "vocalist",
    "screamer",   "growler",      "rocker",      "metalhead",   "headbanger",
    "shredder",   "axeman",       "drummer",     "bassist",     "player",
    "master",     "lord",         "king",        "prince",      "duke",
    "baron",      "knight",       "paladin",     "warrior",     "fighter",
    "soldier",    "mercenary",    "assassin",    "slayer",      "reaper",
    "harvester",  "collector",    "seeker",      "finder",      "keeper",
    "guardian",   "warden",       "watchman",    "sentinel",    "protector",
    "defender",   "hero",         "villain",     "antihero",    "rogue",
    "bandit",     "outlaw",       "pirate",      "corsair",     "buccaneer",
    "marauder",   "raider",       "pillager",    "plundered",   "sacked",
    "ruined",     "destroyed",    "broken",      "shattered",   "crushed",
    "smashed",    "pounded",      "beaten",      "lashed",      "torn",
    "ripped",     "sliced",       "diced",       "cut",         "bled",
    "died",       "lived",        "breathed",    "existed",     "survived",
    "perished",   "vanished",     "dissolved",   "evaporated",  "melted",
    "charred",    "blackened",    "scorched",    "seared",      "riff",
    "mosh",       "thrash",       "grind",       "morbid",      "entombed",
    "obituary",   "sepulchre",    "mortuary",    "pathology",   "autopsy",
    "necropsy",   "gangrene",     "sepsis",      "malignant",   "tumor",
    "cancer",     "damian",       "tyrant",      "goliath",     "ajax",
    "behemoth",   "krusher",      "brutus",      "morgath",     "vanguard",
    "titan",      "colossus",     "hammer",      "bastion",     "ironclad",
    "rampart",    "bulwark",      "garrison",    "fortitude",   "valour",
    "conqueror",  "warlord",      "overlord",    "dominion",    "command",
    "general",    "captain",      "marshal",     "commander",   "centurion",
    "legion",     "phalanx",      "cohort",      "regiment",    "trooper",
    "striker",    "blaster",      "cannon",      "mortar",      "howitzer",
    "artillery",  "ballistic",    "missile",     "torpedo",     "detonator",
    "grenade",    "dynamite",     "c4",          "fuse",        "spark",
    "charge",     "voltage",      "ampere",      "watt",        "frequency",
    "oscillator", "resonator",    "amplifier",   "distortion",  "overdrive",
    "feedback",   "speaker",      "cabinet",     "stack",       "head",
    "pedal",      "switch",       "jack",        "plug",        "cable",
    "string",     "pick",         "fret",        "neck",        "bridge",
    "pickup",     "whammy",       "tremolo",     "vibrato",     "solo",
    "riff",       "breakdown",    "blast",       "doublebass",  "cymbal",
    "snare",      "tom",          "bass",        "groove",      "rhythm",
    "tempo",      "beat",         "pulse",       "heartbeat",   "adrenaline",
    "surge",      "rush",         "speed",       "velocity",    "momentum",
    "inertia",    "gravity",      "mass",        "density",     "alloy",
    "chrome",     "titanium",     "cobalt",      "tungsten",    "platinum",
    "bronze",     "brass",        "copper",      "zinc",        "carbon",
    "graphite",   "silicon",      "quartz",      "granite",     "slate",
    "basalt",     "shale",        "clay",        "mud",         "dirt",
    "soil",       "ground",       "earth",       "world",       "sphere",
    "globe",      "orbit",        "trajectory",  "comet",       "meteor",
    "asteroid",   "impact",       "crater",      "shockwave",   "tremor",
    "quake",      "seismic",      "tectonic",    "faultline",   "ridge",
    "cliff",      "precipice",    "chasm",       "ravine",      "canyon",
    "valley",     "basin",        "trench",      "channel",     "tunnel",
    "shaft",      "pipeline",     "conduit",     "valve",       "gauge",
    "dial",       "lever",        "switch",      "button",      "trigger",
    "hammer",     "bolt",         "screw",       "nut",         "washer",
    "rivet",      "weld",         "solder",      "flux",        "arc",
    "torch",      "burner",       "heater",      "cooler",      "radiator",
    "vent",       "exhaust",      "intake",      "compressor",  "turbine",
    "engine",     "motor",        "piston",      "cylinder",    "valve",
    "camshaft",   "crankshaft",   "manifold",    "throttle",    "clutch",
    "gear",       "transmission", "axle",        "wheel",       "tread",
    "track",      "caterpillar",  "bulldozer",   "excavator",   "crane",
    "hoist",      "winch",        "cable",       "rope",        "wire",
    "mesh",       "net",          "grid",        "lattice",     "matrix",
    "structure",  "framework",    "skeleton",    "frame",       "chassis",
    "hull",       "armor",        "plating",     "shield",      "barrier",
    "blockade",   "barricade",    "fence",       "gate",        "portal",
    "door",       "hatch",        "airlock",     "valkyrie",    "valour",
    "viking",     "spartan",      "gladiator",   "berserker",   "centurion",
    "templar",    "crusader",     "ronin",       "samurai",     "ninja",
    "assassin",   "hunter",       "tracker",     "scout",       "ranger",
    "sniper",     "marksman",     "gunner",      "pilot",       "driver",
    "operator",   "mechanic",     "engineer",    "technician",  "builder",
    "maker",      "creator",      "architect",   "designer",    "planner",
    "strategist", "tactician",    "commander",   "leader",      "boss",
    "chief",      "head",         "alpha",       "apex",        "prime",
    "max",        "ultra",        "super",       "hyper",       "mega",
    "giga",       "tera",         "peta",        "exas",        "absolute",
    "total",      "complete",     "infinite",    "endless",     "boundless",
    "limitless",  "vast",         "immense",     "massive",     "gigantic",
    "colossal",   "monstrous",    "titanic",     "epic",        "legendary",
    "mythic",     "fabled",       "fame",        "glory",       "honor",
    "pride",      "dignity",      "respect",     "tribute",     "triumph",
    "victory",    "win",          "success",     "mastery",     "dominance",
    "power",      "might",        "strength",    "force",       "energy",
    "vigor",      "vitality",     "stamina",     "endurance",   "resilience",
    "tenacity",   "grit",         "resolve",     "will",        "mind",
    "focus",      "intent",       "purpose",     "mission",     "quest",
    "journey",    "voyage",       "expedition",  "safari",      "hike",
    "trek",       "march",        "parade",      "procession",  "rally",
    "gathering",  "assembly",     "meeting",     "council",     "board",
    "panel",      "bureau",       "agency",      "syndicate",   "cartel",
    "league",     "alliance",     "coalition",   "union",       "guild",
    "clan",       "tribe",        "faction",     "party",       "crew",
    "gang",       "mob",          "swarm",       "horde",       "pack",
    "herd",       "flock",        "school",      "fleet",       "armada",
    "navy",       "airforce",     "marines",     "infantry",    "cavalry",
    "artillery",  "division",     "brigade",     "battalion",   "company",
    "platoon",    "squad",        "team",        "unit",        "element",
    "cell",       "base",         "post",        "station",     "outpost",
    "camp",       "headquarters", "command",     "control",     "hub",
    "node",       "point",        "apex",        "vertex",      "zenith",
    "summit",     "peak",         "crest",       "ridge",       "crown",
    "cap",        "tip",          "edge",        "blade",       "point",
    "spike",      "thorn",        "barb",        "hook",        "claw",
    "talon",      "tooth",        "fang",        "jaw",         "skull",
    "bone",       "spine",        "rib",         "chest",       "heart",
    "pulse",      "blood",        "vein",        "artery",      "muscle",
    "sinew",      "tendon",       "fiber",       "strand",      "thread",
    "wire",       "cable",        "cord",        "rope",        "line",
    "stripe",     "streak",       "beam",        "ray",         "shaft",
    "column",     "pillar",       "post",        "pole",        "rod",
    "bar",        "beam",         "joist",       "girder",      "beam",
    "strut",      "brace",        "anchor",      "mooring",     "dock",
    "pier",       "port",         "harbor",      "bay",         "gulf",
    "sea",        "ocean",        "deep",        "abyss",       "void",
    "space",      "realm",        "domain",      "empire",      "kingdom",
    "reign",      "rule",         "law",         "order",       "command",
    "edict",      "decree",       "code",        "rule",        "maxim",
    "axiom",      "theorem",      "proof",       "logic",       "reason",
    "mind",       "brain",        "thought",     "idea",        "concept",
    "plan",       "design",       "blueprint",   "scheme",      "plot",
    "draft",      "sketch",       "outline",     "frame",       "structure",
    "build",      "form",         "shape",       "figure",      "model",
    "pattern",    "template",     "matrix",      "grid",        "network",
    "web",        "mesh",         "net",         "trap",        "snare",
    "catch",      "haul",         "load",        "cargo",       "freight",
    "shipment",   "supply",       "stock",       "store",       "reserve",
    "pool",       "bank",         "fund",        "wealth",      "treasure",
    "gold",       "silver",       "metal",       "iron",        "steel",
    "alloy",      "chrome",       "titanium",    "copper",      "brass",
    "bronze",     "zinc",         "carbon",      "stone",       "rock",
    "crystal",    "gem",          "jewel",       "diamond",     "ruby",
    "emerald",    "sapphire",     "topaz",       "garnet",      "jade",
    "amber",      "pearl",        "coral",       "shell",       "bone",
    "horn",       "antler",       "tusk",        "claw",        "fang",
    "beak",       "talon",        "feather",     "wing",        "tail",
    "scale",      "skin",         "hide",        "fur",         "mane",
    "crest",      "spike",        "thorn",       "brier",       "brush",
    "thicket",    "forest",       "woods",       "grove",       "timber",
    "lumber",     "log",          "stump",       "root",        "branch",
    "bough",      "twig",         "leaf",        "frond",       "fern",
    "moss",       "lichen",       "fungus",      "mold",        "spore",
    "seed",       "pod",          "nut",         "fruit",       "berry",
    "flower",     "bloom",        "petal",       "stem",        "stalk",
    "trunk",      "bark",         "sap",         "resin",       "gum",
    "tar",        "pitch",        "asphalt",     "bitumen",     "oil",
    "fuel",       "gas",          "diesel",      "petrol",      "crude",
    "grease",     "lubricant",    "fluid",       "liquid",      "water",
    "steam",      "vapor",        "smoke",       "soot",        "ash",
    "dust",       "dirt",         "mud",         "clay",        "silt",
    "sand",       "gravel",       "pebble",      "stone",       "rock",
    "boulder",    "cliff",        "crag",        "ledge",       "shelf",
    "wall",       "barrier",      "dam",         "dyke",        "levee",
    "mound",      "hill",         "ridge",       "crest",       "peak",
    "summit",     "top",          "crown",       "cap",         "hood",
    "mask",       "helm",         "helmet",      "visor",       "shield",
    "crest",      "banner",       "flag",        "ensign",      "colors",
    "patch",      "badge",        "crest",       "seal",        "stamp",
    "mark",       "brand",        "tag",         "label",       "sign",
    "symbol",     "glyph",        "rune",        "sigil",       "token",
    "coin",       "medal",        "cross",       "star",        "sun",
    "moon",       "planet",       "comet",       "meteor",      "orbit",
    "sphere",     "globe",        "world",       "earth",       "land",
    "ground",     "soil",         "dust",        "clay",        "mud",
    "stone",      "rock",         "iron",        "steel",       "metal",
    "heavy",      "rock",         "doom",        "blast",       "grind",
    "riff",       "shred",        "headbang",    "mosh",        "thrash",
    "surge",      "pulse",        "drive",       "OVERLORD",    "VALKYRIE",
    "VULCAN",     "WARMEYER",     "STRIKER",     "GOLIATH",     "IRONHIDE",
    "SHATTER"};

const char *boot_phrases[] = {
    "RockOS is awake.\n",
    "Welcome to the noise.\n",
    "Kernel loaded. Volume: MAXIMUM.\n",
    "The machine lives.\n",
    "Boot sequence complete. Let the riff begin.\n",
    "System online. Damian is watching.\n",
    "Tyrant has entered the system.\n",
    "Welcome back, rocker.\n",
    "RockOS has risen from the static.\n",
};

const char *command_not_found_phrases[] = {
    "That riff doesn't exist.\n",
    "Unknown command. Try something that actually rocks.\n",
    "Damian: What the hell is that supposed to be?\n",
    "Tyrant: That command is beneath this machine.\n",
    "Static. Nothing but static.\n",
    "No such command. Tune your damn input.\n",
    "That isn't a command. That's noise.\n",
};
const char *ls_phrases[] = {
    "Let's see what you've got.\n",
    "Scanning the riff chamber...\n",
    "Directory loaded. Don't lose the beat.\n",
    "Here's what survived the last breakdown:\n",
    "The filesystem speaks:\n",
    "RockOS // Listing the wreckage:\n",
    "Tyrant checked the directory. It approves.\n",
    "Damian says there's something interesting in here:\n",
    "Files incoming. Try not to break anything.\n",
    "Opening the vault...\n",
    "Directory contents, straight from the underground:\n",
    "Let's see what's lurking in there:\n",
    "Filesystem scan complete. Here's the loot:\n",
    "The shelves are loaded:\n",
    "RockOS found these riffs:\n",
};

const char *cd_phrases[] = {
    "Moving through the filesystem...\n",
    "New territory. Don't get lost.\n",
    "Damian knows the way.\n",
    "Tyrant is taking you deeper.\n",
    "Changing coordinates...\n",
    "Entering new territory.\n",
    "Path changed. Keep moving.\n",
    "RockOS // Location shifted.\n",
    "Descending into the filesystem...\n",
    "New directory. Same chaos.\n",
    "The path bends. RockOS follows.\n",
    "Moving deeper into the machine...\n",
    "Coordinates updated.\n",
    "Another room in the machine.\n",
    "Keep walking. The riff isn't over.\n",
};

const char *pwd_phrases[] = {
    "You are standing here:\n",
    "Current position:\n",
    "RockOS knows where you are:\n",
    "Your coordinates, rocker:\n",
    "Damian checked the map:\n",
    "Tyrant knows your location:\n",
    "Filesystem coordinates locked:\n",
    "This is where the riff currently lives:\n",
    "Location confirmed:\n",
    "You are here. Obviously.\n",
    "RockOS // Current territory:\n",
    "Map updated. Current location:\n",
    "The machine has your position:\n",
    "Your place in the filesystem:\n",
    "Coordinates confirmed. Don't wander too far.\n",
};

const char *mkdir_phrases[] = {
    "New territory forged.\n",
    "Directory forged in iron.\n",
    "A new chamber has been built.\n",
    "Damian approves the construction.\n",
    "Tyrant just built another fortress.\n",
    "New directory. New possibilities.\n",
    "The filesystem grows stronger.\n",
    "Another room joins the machine.\n",
    "Structure created. Rock on.\n",
    "New territory unlocked.\n",
    "The foundation is laid.\n",
    "RockOS has expanded its domain.\n",
    "A new stronghold rises.\n",
    "Directory hammered into existence.\n",
    "Built from nothing. Like every good riff.\n",
};

const char *touch_phrases[] = {
    "File forged.\n",
    "Fresh file. Still warm from the forge.\n",
    "A new riff has been written to disk.\n",
    "Damian left something behind.\n",
    "Tyrant forged a new artifact.\n",
    "File created. Make it useful.\n",
    "Another piece of the machine comes alive.\n",
    "New file. No excuses now.\n",
    "RockOS has carved another mark into the disk.\n",
    "The filesystem just got heavier.\n",
    "Fresh metal on the filesystem.\n",
    "A new artifact has entered the vault.\n",
    "File forged without mercy.\n",
    "Another byte enters the chaos.\n",
    "Created. Now give it a purpose.\n",
};

const char *rm_phrases[] = {
    "File obliterated.\n",
    "That file has left the building.\n",
    "Deleted. No encore.\n",
    "Damian pulled the plug.\n",
    "Tyrant says it won't be missed.\n",
    "The file has been crushed.\n",
    "Gone. Reduced to filesystem dust.\n",
    "One less artifact in the vault.\n",
    "File erased from the riff.\n",
    "The disk has been cleansed.\n",
    "Deleted with extreme prejudice.\n",
    "No backup. No mercy. No file.\n",
    "That file just got the breakdown treatment.\n",
    "RockOS has forgotten it ever existed.\n",
    "File destroyed. Moving on.\n",
};

const char *rmdir_phrases[] = {
    "Directory demolished.\n",
    "The fortress has fallen.\n",
    "Another chamber reduced to rubble.\n",
    "Damian brought the hammer.\n",
    "Tyrant has conquered the territory.\n",
    "Directory wiped from existence.\n",
    "One less room in the machine.\n",
    "The filesystem just lost a stronghold.\n",
    "Chamber destroyed. Nothing left behind.\n",
    "The walls came down.\n",
    "RockOS has cleared the battlefield.\n",
    "Directory crushed beneath the riff.\n",
    "Fortress dismantled.\n",
    "The old territory is gone.\n",
    "No directory. No survivors.\n",
};

const char *cat_phrases[] = {
    "Reading the artifact...\n",
    "Let's hear what this file has to say:\n",
    "Damian is reading the inscription:\n",
    "Tyrant opened the archive:\n",
    "Decoding the riff...\n",
    "Pulling the words from the disk:\n",
    "Reading straight from the machine:\n",
    "The file speaks:\n",
    "Opening the vault:\n",
    "Here's what survived on disk:\n",
    "RockOS // File contents:\n",
    "Let's see what you wrote:\n",
    "Reading the ancient scroll...\n",
    "The filesystem has something to say:\n",
    "Opening the artifact. Stand by:\n",
};

const char *echo_phrases[] = {
    "RockOS heard you:\n",
    "Echo from the void:\n",
    "The machine answers:\n",
    "Damian repeats:\n",
    "Tyrant echoes:\n",
    "Your words hit the speakers:\n",
    "Signal received:\n",
    "The static responds:\n",
    "RockOS // Transmission received:\n",
    "Your voice survived the kernel:\n",
    "Message amplified:\n",
    "The riff echoes back:\n",
    "Input received loud and clear:\n",
    "The machine heard every word:\n",
    "Signal locked. Echo incoming:\n",
};

const char *clear_phrases[] = {
    "Clearing the stage...\n",
    "Wiping the noise...\n",
    "Damian cleared the board.\n",
    "Tyrant demands a clean screen.\n",
    "Static cleared.\n",
    "The stage is clean.\n",
    "RockOS // Fresh canvas.\n",
    "Clearing the wreckage...\n",
    "Screen wiped. Let the riff continue.\n",
    "Nothing but darkness now.\n",
    "The noise has been silenced.\n",
    "Clean slate. Hit the next command.\n",
    "Terminal reset. Volume restored.\n",
    "The screen has entered the void.\n",
    "RockOS has cleared the battlefield.\n",
};

const char *reboot_phrases[] = {
    "Rebooting. See you on the other side.\n",
    "Damian is restarting the machine.\n",
    "Tyrant pulled the emergency lever.\n",
    "The riff stops... temporarily.\n",
    "Restart sequence engaged.\n",
    "RockOS is going dark.\n",
    "Power cycling the beast...\n",
    "The machine needs another breakdown.\n",
    "Rebooting the forge.\n",
    "System shutdown incoming.\n",
    "The kernel is taking five.\n",
    "RockOS // One last chord before reboot.\n",
    "Restarting. Don't touch anything.\n",
    "The machine sleeps. The machine returns.\n",
    "Until the next boot, rocker.\n",
};

const char *uptime_phrases[] = {
    "The machine has been raging for:\n",
    "RockOS has been alive for:\n",
    "Time since the last breakdown:\n",
    "Damian reports machine uptime:\n",
    "Tyrant has been awake for:\n",
    "The beast has been running for:\n",
    "Kernel survival time:\n",
    "RockOS // Time spent making noise:\n",
    "The system has endured:\n",
    "Runtime from the depths:\n",
    "The forge has been burning for:\n",
    "Time since the riff began:\n",
    "System activity duration:\n",
    "The machine refuses to die. Uptime:\n",
    "RockOS has survived this long:\n",
};

const char *damian_phrases[] = {
    "Damian: You called?\n",
    "Damian: Finally. Something interesting.\n",
    "Damian: Keep the machine loud.\n",
    "Damian: Don't break my filesystem.\n",
    "Damian: I've seen worse kernels.\n",
    "Damian: Now THAT is a command.\n",
    "Damian: You're getting the hang of this.\n",
    "Damian: The machine is listening.\n",
    "Damian: Keep pushing it.\n",
    "Damian: RockOS doesn't sleep.\n",
    "Damian: If it crashes, call it a feature.\n",
    "Damian: Less talking. More hacking.\n",
    "Damian: The forge is still burning.\n",
    "Damian: Make something worth remembering.\n",
    "Damian: That's enough noise. For now.\n",
};

const char *tyrant_phrases[] = {
    "Tyrant: You have entered my domain.\n",
    "Tyrant: Command accepted.\n",
    "Tyrant: Proceed.\n",
    "Tyrant: The machine obeys.\n",
    "Tyrant: Another command. Another victory.\n",
    "Tyrant: Do not waste the machine's time.\n",
    "Tyrant: RockOS stands.\n",
    "Tyrant: The filesystem is under control.\n",
    "Tyrant: Keep moving.\n",
    "Tyrant: Weak commands will be ignored.\n",
    "Tyrant: The kernel remains undefeated.\n",
    "Tyrant: You want power? Earn it.\n",
    "Tyrant: The forge does not stop.\n",
    "Tyrant: Chaos is acceptable. Corruption is not.\n",
    "Tyrant: Continue.\n",
};

const char *help_phrases[] = {
    "RockOS doesn't need a manual. It needs a damn volume knob.\n"
    "Available commands: ls, cd, pwd, mkdir, rmdir, touch, rm, cat, echo,\n"
    "clear, uptime, reboot, help, and tyrant. Use 'command <args>' to unleash them.\n",

    "Welcome to the RockOS command deck.\n"
    "ls shows what's here. cd moves you through the filesystem. pwd tells you where you stand.\n"
    "mkdir builds directories. rmdir tears down empty ones. touch creates files.\n"
    "cat reads them. rm destroys them. echo spits text into the terminal.\n"
    "clear cleans the stage. uptime tells you how long the machine has been screaming.\n"
    "reboot restarts the show. And tyrant... well, you'll figure that one out.\n",

    "You have entered the RockOS shell.\n"
    "Filesystem commands: ls, cd, pwd, mkdir, rmdir, touch, rm, cat.\n"
    "Terminal commands: clear, echo, uptime, reboot.\n"
    "System commands: help, tyrant.\n"
    "If a command fails, don't blame the kernel. Check your damn syntax first.\n",

    "Damian's command board:\n"
    "  ls       - List the contents of the current directory.\n"
    "  cd       - Move into another directory.\n"
    "  pwd      - Show your current path.\n"
    "  mkdir    - Forge a new directory.\n"
    "  rmdir    - Remove an empty directory.\n"
    "  touch    - Create a new file.\n"
    "  rm       - Delete a file.\n"
    "  cat      - Read a file's contents.\n"
    "  echo     - Throw text into the terminal.\n"
    "  clear    - Wipe the terminal clean.\n"
    "  uptime   - See how long RockOS has been alive.\n"
    "  reboot   - Restart the machine.\n"
    "  help     - You already know what this does.\n"
    "  tyrant   - ...you sure about that?\n",

    "The shell is simple. The attitude isn't.\n"
    "Navigate with cd. Inspect with ls. Locate yourself with pwd.\n"
    "Create with mkdir and touch. Destroy with rm and rmdir.\n"
    "Read files with cat. Speak through echo. Clear the stage with clear.\n"
    "Check uptime when you want to know how long the kernel has survived.\n"
    "Use reboot when you're ready to kick the machine back into the pit.\n",

    "RockOS commands are loaded and waiting.\n"
    "Treat the filesystem like a backstage area: know where you're going before you start tearing things down.\n"
    "Use 'ls' to inspect. 'cd' to travel. 'pwd' to orient yourself.\n"
    "Use 'mkdir' and 'touch' to create. Use 'rm' and 'rmdir' to destroy.\n"
    "Use 'cat' to read and 'echo' to write your voice into the terminal.\n"
    "The rest of the commands keep the machine itself alive.\n"
};

constexpr int HELP_PHRASE_COUNT =
    sizeof(help_phrases) / sizeof(help_phrases[0]);

constexpr int METAL_WORD_COUNT =
    sizeof(metal_word_list) / sizeof(metal_word_list[0]);

constexpr int BOOT_WORD_COUNT = sizeof(boot_phrases) / sizeof(boot_phrases[0]);

constexpr int COMMAND_NOT_FOUND_PHRASE_COUNT =
    sizeof(command_not_found_phrases) / sizeof(command_not_found_phrases[0]);

constexpr int LS_PHRASE_COUNT = sizeof(ls_phrases) / sizeof(ls_phrases[0]);

constexpr int CD_PHRASE_COUNT = sizeof(cd_phrases) / sizeof(cd_phrases[0]);

constexpr int PWD_PHRASE_COUNT = sizeof(pwd_phrases) / sizeof(pwd_phrases[0]);

constexpr int MKDIR_PHRASE_COUNT =
    sizeof(mkdir_phrases) / sizeof(mkdir_phrases[0]);

constexpr int TOUCH_PHRASE_COUNT =
    sizeof(touch_phrases) / sizeof(touch_phrases[0]);

constexpr int RM_PHRASE_COUNT = sizeof(rm_phrases) / sizeof(rm_phrases[0]);

constexpr int RMDIR_PHRASE_COUNT =
    sizeof(rmdir_phrases) / sizeof(rmdir_phrases[0]);

constexpr int CAT_PHRASE_COUNT = sizeof(cat_phrases) / sizeof(cat_phrases[0]);

constexpr int ECHO_PHRASE_COUNT =
    sizeof(echo_phrases) / sizeof(echo_phrases[0]);

constexpr int CLEAR_PHRASE_COUNT =
    sizeof(clear_phrases) / sizeof(clear_phrases[0]);

constexpr int REBOOT_PHRASE_COUNT =
    sizeof(reboot_phrases) / sizeof(reboot_phrases[0]);

constexpr int UPTIME_PHRASE_COUNT =
    sizeof(uptime_phrases) / sizeof(uptime_phrases[0]);

constexpr int DAMIAN_PHRASE_COUNT =
    sizeof(damian_phrases) / sizeof(damian_phrases[0]);

constexpr int TYRANT_PHRASE_COUNT =
    sizeof(tyrant_phrases) / sizeof(tyrant_phrases[0]);
