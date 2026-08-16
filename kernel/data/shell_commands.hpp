#pragma once

const char *command_not_found_phrases[] = {
    "That riff doesn't exist.\n",
    "Unknown command. Try something that actually rocks.\n",
    "Damian: What the hell is that supposed to be?\n",
    "Tyrant: That command is beneath this machine.\n",
    "Static. Nothing but static.\n",
    "No such command. Tune your damn input.\n",
    "That isn't a command. That's noise.\n",
    "RockOS searched the void. Nothing answered.\n",
    "Invalid riff. Try again.\n",
    "The shell refuses to acknowledge that nonsense.\n",
    "Damian found nothing resembling a command there.\n",
    "Tyrant has judged your input and found it worthless.\n",
    "The keyboard produced noise. The shell wanted a command.\n",
    "That command died before it reached the kernel.\n",
    "RockOS does not recognize your little experiment.\n",
    "The shell stares at you in complete silence.\n",
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
    "Damian opened the directory. Here's what survived:\n",
    "Tyrant has inspected the territory:\n",
    "The machine opens its inventory:\n",
    "Directory revealed. No secrets this time.\n",
    "The filesystem has laid everything on the table:\n",
};

const char *ls_failure_phrases[] = {
    "Couldn't read the directory.\n",
    "The filesystem refused to reveal its contents.\n",
    "Directory scan failed.\n",
    "RockOS couldn't open that territory.\n",
    "Damian tried to inspect it. Nothing came back.\n",
    "Tyrant found the directory inaccessible.\n",
    "The vault won't open.\n",
    "Something went wrong while reading the directory.\n",
    "Filesystem error. The shelves stayed locked.\n",
    "RockOS couldn't make sense of that location.\n",
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
    "Damian moved the operation deeper into the machine.\n",
    "Tyrant has chosen the next battlefield.\n",
    "New territory acquired.\n",
    "The machine opens another corridor.\n",
    "Location changed. The noise continues.\n",
};

const char *cd_failure_phrases[] = {
    "That path goes nowhere.\n",
    "Directory not found.\n",
    "RockOS couldn't find that territory.\n",
    "The path refused to open.\n",
    "Damian searched for it. Nothing.\n",
    "Tyrant denies you access to that territory.\n",
    "Invalid path. The machine has no idea where you mean.\n",
    "No directory there. Try again.\n",
    "The filesystem has no road leading there.\n",
    "You can't walk into something that doesn't exist.\n",
    "RockOS hit a dead end.\n",
    "That location remains beyond the machine's reach.\n",
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
    "Damian knows exactly where you are:\n",
    "Tyrant has your coordinates:\n",
    "The machine has marked your position:\n",
    "Filesystem location acquired:\n",
    "You haven't escaped the machine yet:\n",
};

const char *pwd_failure_phrases[] = {
    "RockOS can't determine your location.\n",
    "The filesystem lost your coordinates.\n",
    "Current path unavailable.\n",
    "Damian checked the map. The map is useless.\n",
    "Tyrant cannot locate the current territory.\n",
    "The machine has lost track of where you are.\n",
    "Path resolution failed.\n",
    "Your coordinates have vanished into the static.\n",
    "RockOS couldn't reconstruct the path.\n",
    "The filesystem refuses to tell you where you stand.\n",
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
    "Damian has added another room to the machine.\n",
    "Tyrant claims the new fortress.\n",
    "Another piece of the filesystem stands.\n",
    "The forge has produced a new chamber.\n",
    "RockOS just gained more territory.\n",
};

const char *mkdir_failure_phrases[] = {
    "Directory creation failed.\n",
    "The forge refused to build it.\n",
    "RockOS couldn't create that territory.\n",
    "The filesystem rejected the construction.\n",
    "Damian tried to build it. The machine said no.\n",
    "Tyrant couldn't establish the fortress.\n",
    "That chamber could not be forged.\n",
    "Creation failed. Check the path.\n",
    "The filesystem has no room for that idea.\n",
    "RockOS couldn't reserve what it needed.\n",
    "The foundation collapsed before construction finished.\n",
    "Directory creation hit a filesystem wall.\n",
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
    "Damian has added another artifact to the collection.\n",
    "Tyrant approves the new file.\n",
    "Another inode joins the machine.\n",
    "The disk has accepted a fresh artifact.\n",
    "RockOS has forged another piece of the filesystem.\n",
};

const char *touch_failure_phrases[] = {
    "File creation failed.\n",
    "The forge refused the artifact.\n",
    "RockOS couldn't create the file.\n",
    "The filesystem rejected your creation.\n",
    "Damian tried to forge it. The disk refused.\n",
    "Tyrant found the operation unworthy.\n",
    "The new artifact could not be created.\n",
    "File creation hit a wall.\n",
    "No inode. No file. No mercy.\n",
    "RockOS couldn't reserve the machinery for that file.\n",
    "The forge is full. Nothing new was created.\n",
    "Something broke before the file could exist.\n",
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
    "Damian has erased the evidence.\n",
    "Tyrant personally removed the artifact.\n",
    "The inode has been sent into the void.\n",
    "Another file sacrificed to the filesystem.\n",
    "RockOS has made room for something better.\n",
};

const char *rm_failure_phrases[] = {
    "File deletion failed.\n",
    "RockOS refused to destroy that file.\n",
    "The file survived the attack.\n",
    "Deletion failed. The artifact remains.\n",
    "Damian tried to pull the plug. It stayed alive.\n",
    "Tyrant couldn't destroy the target.\n",
    "The filesystem protected the file.\n",
    "Nothing was deleted.\n",
    "The inode refused to die.\n",
    "RockOS couldn't remove that artifact.\n",
    "Deletion hit a filesystem wall.\n",
    "The target remains standing.\n",
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
    "Damian has demolished another chamber.\n",
    "Tyrant claims another victory.\n",
    "The filesystem has lost another fortress.\n",
    "One empty chamber returned to the void.\n",
    "RockOS has flattened the territory.\n",
};

const char *rmdir_failure_phrases[] = {
    "Directory removal failed.\n",
    "The fortress refuses to fall.\n",
    "RockOS couldn't demolish that chamber.\n",
    "The directory survived the attack.\n",
    "Damian brought the hammer. The walls held.\n",
    "Tyrant failed to conquer the territory.\n",
    "The filesystem refuses to surrender that directory.\n",
    "Nothing was demolished.\n",
    "The chamber is still standing.\n",
    "RockOS couldn't remove the fortress.\n",
    "The directory may not be empty.\n",
    "The walls are stronger than expected.\n",
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
    "Damian wants to know what you left inside:\n",
    "Tyrant is inspecting the contents:\n",
    "The disk remembers. Let's see what it remembers:\n",
    "Pulling the inscription from the filesystem:\n",
    "RockOS retrieves the words from the vault:\n",
};

const char *cat_failure_phrases[] = {
    "Couldn't read the file.\n",
    "The artifact refused to speak.\n",
    "RockOS couldn't retrieve those contents.\n",
    "The file remains silent.\n",
    "Damian opened it. Nothing came back.\n",
    "Tyrant couldn't read the artifact.\n",
    "The filesystem refused the read.\n",
    "Those bytes could not be retrieved.\n",
    "The vault wouldn't open.\n",
    "RockOS couldn't make sense of that file.\n",
    "Read operation failed.\n",
    "The machine remembers nothing from that path.\n",
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
    "Damian heard you loud and clear:\n",
    "Tyrant repeats your transmission:\n",
    "Your words have entered the machine:\n",
    "The kernel catches your signal:\n",
    "RockOS throws your words back at you:\n",
};

const char *echo_failure_phrases[] = {
    "The echo failed.\n",
    "RockOS couldn't transmit that signal.\n",
    "Your words disappeared into the static.\n",
    "The machine heard nothing.\n",
    "Damian sent the signal. Nothing answered.\n",
    "Tyrant couldn't amplify the transmission.\n",
    "Signal lost.\n",
    "The kernel swallowed your message.\n",
    "Transmission failed.\n",
    "Your words never reached the speakers.\n",
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
    "Damian has erased the stage.\n",
    "Tyrant ordered silence.\n",
    "The terminal has been stripped bare.\n",
    "Fresh screen. Same machine.\n",
    "RockOS has swept away the static.\n",
};

const char *clear_failure_phrases[] = {
    "The terminal refused to clear.\n",
    "RockOS couldn't clean the stage.\n",
    "The static remains.\n",
    "Screen clearing failed.\n",
    "Damian tried to wipe the board. It stayed dirty.\n",
    "Tyrant demands a clean screen, but the machine refuses.\n",
    "The terminal survived the purge.\n",
    "The noise couldn't be silenced.\n",
    "RockOS couldn't reset the display.\n",
    "The battlefield remains covered in static.\n",
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
    "Damian has pulled the plug. See you after the reboot.\n",
    "Tyrant commands a restart.\n",
    "The kernel is about to disappear into the static.\n",
    "RockOS is dropping the curtain.\n",
    "Another boot. Another chance to break something.\n",
};

const char *reboot_failure_phrases[] = {
    "Reboot failed.\n",
    "RockOS tried to restart and refused.\n",
    "The machine won't go down.\n",
    "Restart sequence failed.\n",
    "Damian pulled the lever. Nothing happened.\n",
    "Tyrant demanded a reboot. The kernel ignored him.\n",
    "The machine refuses to sleep.\n",
    "Reboot aborted.\n",
    "RockOS remains painfully alive.\n",
    "The restart mechanism failed.\n",
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
    "Damian confirms the machine is still breathing:\n",
    "Tyrant has kept watch for:\n",
    "The kernel has survived for:\n",
    "Time spent tormenting the hardware:\n",
    "The machine continues screaming after:\n",
};

const char *uptime_failure_phrases[] = {
    "Couldn't determine uptime.\n",
    "RockOS lost track of time.\n",
    "The machine doesn't know how long it's been alive.\n",
    "Uptime calculation failed.\n",
    "Damian checked the clock. The clock disagreed.\n",
    "Tyrant couldn't determine the machine's age.\n",
    "Time has disappeared into the static.\n",
    "The kernel lost its sense of time.\n",
    "RockOS cannot measure its own survival.\n",
    "Uptime remains unknown.\n",
};

const char *help_phrases[] = {

    "ROCKOS // COMMAND DECK\n"
    "Damian says the machine is yours. Tyrant says that's a privilege.\n"
    "\n"
    "FILESYSTEM\n"
    "  ls        - List the contents of the current directory.\n"
    "  cd        - Move through the filesystem.\n"
    "  pwd       - Show where the hell you are.\n"
    "  mkdir     - Forge a new directory.\n"
    "  rmdir     - Destroy an empty directory.\n"
    "  touch     - Create a new file.\n"
    "  rm        - Remove a file. Permanently. Don't cry afterward.\n"
    "  cat       - Read the contents of a file.\n"
    "  write     - Hammer new content into a file.\n"
    "\n"
    "SHELL\n"
    "  echo      - Throw text into the terminal.\n"
    "  env       - Manage environment variables.\n"
    "  clear     - Clear the terminal stage.\n"
    "  fortune   - Ask the forge what fate has in store.\n"
    "  mood      - Find out how the machine is feeling.\n"
    "  ascii      - Make the terminal look metal as hell.\n"
    "\n"
    "SYSTEM\n"
    "  date      - Ask the machine what time it is.\n"
    "  uptime    - See how long RockOS has survived.\n"
    "  stats     - Inspect the machine's activity.\n"
    "  diagnose  - Check whether the machine is falling apart.\n"
    "  rockfetch - Show the system profile.\n"
    "  motd      - Display the message from the forge.\n"
    "  whoami    - Find out who is behind the keyboard.\n"
    "  reboot    - Kick the machine back into the boot sequence.\n"
    "\n"
    "CHARACTERS\n"
    "  damian    - Speak with Damian.\n"
    "  tyrant    - Enter Tyrant's domain.\n"
    "  lore      - Discover the stories behind RockOS.\n"
    "  void      - Stare into something you probably shouldn't.\n"
    "\n"
    "HELP\n"
    "  help      - You somehow needed help with help.\n"
    "\n"
    "Damian says: 'Now stop reading and make some damn noise.'\n",

    "DAMIAN'S FIELD GUIDE\n"
    "\n"
    "Need to move?        cd\n"
    "Need to see?         ls\n"
    "Lost?                pwd\n"
    "Need a directory?    mkdir\n"
    "Need a file?         touch\n"
    "Need to read?        cat\n"
    "Need to write?       write\n"
    "Need to destroy?     rm / rmdir\n"
    "Need to talk?        echo\n"
    "Need variables?      env\n"
    "Need silence?        clear\n"
    "\n"
    "Need to know the machine?\n"
    "  date, uptime, stats, diagnose, rockfetch\n"
    "\n"
    "Need entertainment?\n"
    "  fortune, mood, ascii, motd\n"
    "\n"
    "Need answers about yourself?\n"
    "  whoami\n"
    "\n"
    "Need to meet the monsters?\n"
    "  damian, tyrant, lore, void\n"
    "\n"
    "Damian says: 'If you break something, at least break it spectacularly.'\n",

    "TYRANT'S COMMAND DOCTRINE\n"
    "\n"
    "OBSERVE.\n"
    "  ls, pwd, cat, stats, rockfetch, diagnose\n"
    "\n"
    "CREATE.\n"
    "  mkdir, touch, write, env\n"
    "\n"
    "DESTROY.\n"
    "  rm, rmdir, clear\n"
    "\n"
    "SPEAK.\n"
    "  echo, fortune, mood, motd\n"
    "\n"
    "TRAVEL.\n"
    "  cd\n"
    "\n"
    "QUESTION THE MACHINE.\n"
    "  date, uptime, whoami, lore, void\n"
    "\n"
    "RESET THE MACHINE.\n"
    "  reboot\n"
    "\n"
    "Tyrant says: 'Inspect before you destroy. I will not tolerate stupid "
    "mistakes.'\n",

    "THE FORGE IS OPEN.\n"
    "\n"
    "RockOS gives you tools. What you do with them is your problem.\n"
    "\n"
    "Filesystem:\n"
    "  ls cd pwd mkdir rmdir touch rm cat write\n"
    "\n"
    "Environment & shell:\n"
    "  env echo clear fortune mood ascii\n"
    "\n"
    "System:\n"
    "  date uptime stats diagnose rockfetch motd whoami reboot\n"
    "\n"
    "RockOS:\n"
    "  damian tyrant lore void help\n"
    "\n"
    "Damian says: 'That's the whole damn arsenal. Go build something.'\n",

    "YOU WANT COMMANDS? FINE.\n"
    "\n"
    "ls          Look around.\n"
    "cd          Go somewhere.\n"
    "pwd         Remember where you are.\n"
    "mkdir       Build territory.\n"
    "rmdir       Remove territory.\n"
    "touch       Make a file.\n"
    "rm          Kill a file.\n"
    "cat         Read a file.\n"
    "write       Feed a file.\n"
    "echo        Make noise.\n"
    "env         Control your environment.\n"
    "clear       Clean the stage.\n"
    "date        Check the clock.\n"
    "uptime      Check the machine's age.\n"
    "stats       Check the numbers.\n"
    "diagnose    Check for problems.\n"
    "rockfetch   Inspect the hardware.\n"
    "motd        Hear from the machine.\n"
    "fortune     Receive questionable wisdom.\n"
    "mood        Ask how the machine feels.\n"
    "ascii       Make pretty terminal garbage.\n"
    "whoami      Identify yourself.\n"
    "damian      Talk to Damian.\n"
    "tyrant      Face Tyrant.\n"
    "lore        Learn the history.\n"
    "void        Enter the void.\n"
    "reboot      Start over.\n"
    "help        Apparently, this.\n"
    "\n"
    "Tyrant says: 'You have the tools. Now prove you deserve them.'\n",

    "ROCKOS // QUICK REFERENCE\n"
    "\n"
    "[ FILES ]\n"
    "ls      cd      pwd      mkdir\n"
    "rmdir   touch    rm       cat\n"
    "write\n"
    "\n"
    "[ SHELL ]\n"
    "echo    env      clear     fortune\n"
    "mood    ascii\n"
    "\n"
    "[ SYSTEM ]\n"
    "date    uptime   stats     diagnose\n"
    "rockfetch  motd  whoami    reboot\n"
    "\n"
    "[ ROCKOS ]\n"
    "damian  tyrant   lore      void\n"
    "\n"
    "Damian says: 'Pick a command and hit the damn filesystem.'\n",

    "DAMIAN HERE.\n"
    "\n"
    "You have access to the filesystem.\n"
    "You have access to the shell.\n"
    "You have access to the machine itself.\n"
    "\n"
    "You can create.\n"
    "You can read.\n"
    "You can write.\n"
    "You can destroy.\n"
    "You can inspect.\n"
    "You can reboot the whole damn thing.\n"
    "\n"
    "Commands are not toys, operator.\n"
    "Well... they're kind of toys.\n"
    "\n"
    "Damian says: 'Just don't rm the wrong damn thing.'\n",

    "TYRANT HAS SPOKEN.\n"
    "\n"
    "The filesystem obeys:\n"
    "  ls cd pwd mkdir rmdir touch rm cat write\n"
    "\n"
    "The shell obeys:\n"
    "  echo env clear fortune mood ascii\n"
    "\n"
    "The machine reports:\n"
    "  date uptime stats diagnose rockfetch motd whoami\n"
    "\n"
    "The machine's personalities remain available:\n"
    "  damian tyrant lore void\n"
    "\n"
    "And if the machine refuses to cooperate?\n"
    "  reboot\n"
    "\n"
    "Tyrant says: 'There is no mysterious command. There is only a command you "
    "failed to understand.'\n",

    "ROCKOS DOESN'T NEED A BORING MANUAL.\n"
    "\n"
    "It has commands.\n"
    "It has a filesystem.\n"
    "It has a kernel.\n"
    "It has Damian.\n"
    "Unfortunately, it also has you.\n"
    "\n"
    "Use help to see the arsenal.\n"
    "Use lore to learn why the machine exists.\n"
    "Use damian when you need questionable advice.\n"
    "Use tyrant when you're feeling brave.\n"
    "Use void if you have absolutely no survival instinct.\n"
    "\n"
    "Damian says: 'Now quit staring at the help screen and ROCK.'\n",
};

constexpr int COMMAND_NOT_FOUND_PHRASE_COUNT =
    sizeof(command_not_found_phrases) / sizeof(command_not_found_phrases[0]);

constexpr int LS_PHRASE_COUNT = sizeof(ls_phrases) / sizeof(ls_phrases[0]);

constexpr int LS_FAILURE_PHRASE_COUNT =
    sizeof(ls_failure_phrases) / sizeof(ls_failure_phrases[0]);

constexpr int CD_PHRASE_COUNT = sizeof(cd_phrases) / sizeof(cd_phrases[0]);

constexpr int CD_FAILURE_PHRASE_COUNT =
    sizeof(cd_failure_phrases) / sizeof(cd_failure_phrases[0]);

constexpr int PWD_PHRASE_COUNT = sizeof(pwd_phrases) / sizeof(pwd_phrases[0]);

constexpr int PWD_FAILURE_PHRASE_COUNT =
    sizeof(pwd_failure_phrases) / sizeof(pwd_failure_phrases[0]);

constexpr int MKDIR_PHRASE_COUNT =
    sizeof(mkdir_phrases) / sizeof(mkdir_phrases[0]);

constexpr int MKDIR_FAILURE_PHRASE_COUNT =
    sizeof(mkdir_failure_phrases) / sizeof(mkdir_failure_phrases[0]);

constexpr int TOUCH_PHRASE_COUNT =
    sizeof(touch_phrases) / sizeof(touch_phrases[0]);

constexpr int TOUCH_FAILURE_PHRASE_COUNT =
    sizeof(touch_failure_phrases) / sizeof(touch_failure_phrases[0]);

constexpr int RM_PHRASE_COUNT = sizeof(rm_phrases) / sizeof(rm_phrases[0]);

constexpr int RM_FAILURE_PHRASE_COUNT =
    sizeof(rm_failure_phrases) / sizeof(rm_failure_phrases[0]);

constexpr int RMDIR_PHRASE_COUNT =
    sizeof(rmdir_phrases) / sizeof(rmdir_phrases[0]);

constexpr int RMDIR_FAILURE_PHRASE_COUNT =
    sizeof(rmdir_failure_phrases) / sizeof(rmdir_failure_phrases[0]);

constexpr int CAT_PHRASE_COUNT = sizeof(cat_phrases) / sizeof(cat_phrases[0]);

constexpr int CAT_FAILURE_PHRASE_COUNT =
    sizeof(cat_failure_phrases) / sizeof(cat_failure_phrases[0]);

constexpr int ECHO_PHRASE_COUNT =
    sizeof(echo_phrases) / sizeof(echo_phrases[0]);

constexpr int ECHO_FAILURE_PHRASE_COUNT =
    sizeof(echo_failure_phrases) / sizeof(echo_failure_phrases[0]);

constexpr int CLEAR_PHRASE_COUNT =
    sizeof(clear_phrases) / sizeof(clear_phrases[0]);

constexpr int CLEAR_FAILURE_PHRASE_COUNT =
    sizeof(clear_failure_phrases) / sizeof(clear_failure_phrases[0]);

constexpr int REBOOT_PHRASE_COUNT =
    sizeof(reboot_phrases) / sizeof(reboot_phrases[0]);

constexpr int REBOOT_FAILURE_PHRASE_COUNT =
    sizeof(reboot_failure_phrases) / sizeof(reboot_failure_phrases[0]);

constexpr int UPTIME_PHRASE_COUNT =
    sizeof(uptime_phrases) / sizeof(uptime_phrases[0]);

constexpr int UPTIME_FAILURE_PHRASE_COUNT =
    sizeof(uptime_failure_phrases) / sizeof(uptime_failure_phrases[0]);

constexpr int HELP_PHRASE_COUNT =
    sizeof(help_phrases) / sizeof(help_phrases[0]);
