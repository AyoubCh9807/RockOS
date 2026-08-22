#pragma once

const char *whoami_phrases[] = {
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

const char *motd_messages[] = {
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

const char *rockfetch_labels[] = {
    "OS",     "Kernel", "Architecture", "Shell",     "Filesystem", "CPU",
    "Memory", "Heap",   "Disk",         "Character", "Status",     "Noise",
};

const char *rockfetch_values[] = {
    "RockOS", "0.1",  "x86", "RockShell", "RockFS", "x86 CPU",
    "16 MB",  "4 MB", "ATA", "Tyrant",    "ALIVE",  "MAXIMUM",
};

constexpr int ROCKFETCH_FIELD_COUNT =
    sizeof(rockfetch_labels) / sizeof(rockfetch_labels[0]);

/*
 * Rockfetch command messages.
 */

const char *rockfetch_success_phrases[] = {
    "System profile retrieved. The machine stands tall.\n",
    "Rockfetch complete. RockOS has nothing to hide.\n",
    "System data acquired. Damian would approve.\n",
    "Hardware exposed. The forge is still burning.\n",
    "Rockfetch complete. Everything is where it belongs.\n",
    "System profile loaded. Tyrant has granted you a look.\n",
    "The machine has revealed its specifications.\n",
    "Rockfetch complete. Damian says the machine is healthy.\n",
    "Hardware queried. No smoke detected.\n",
    "Rockfetch complete. Tyrant has inspected the numbers.\n",
    "System profile acquired. Maximum noise confirmed.\n",
    "The machine has spoken. The specifications are yours.\n",
    "Rockfetch complete. Damian is still watching.\n",
    "System information retrieved without incident.\n",
    "Rockfetch complete. Tyrant finds the machine acceptable.\n",
    "The forge has revealed its internals.\n",
    "System profile acquired. Nothing appears to be screaming.\n",
    "Rockfetch complete. Damian approves the architecture.\n",
    "Hardware information retrieved successfully.\n",
    "System profile complete. Tyrant remains unimpressed.\n",
};

constexpr int ROCKFETCH_SUCCESS_PHRASE_COUNT =
    sizeof(rockfetch_success_phrases) / sizeof(rockfetch_success_phrases[0]);

const char *rockfetch_failure_phrases[] = {
    "Rockfetch failed. The machine refused to explain itself.\n",
    "System profile unavailable. Something went quiet.\n",
    "Rockfetch failed. Damian is asking questions.\n",
    "The machine kept its secrets.\n",
    "System information could not be retrieved.\n",
    "Rockfetch failed. Tyrant is not pleased.\n",
    "Hardware inspection failed. The forge is silent.\n",
    "The machine refused the interrogation.\n",
    "Rockfetch failed. Damian has stopped taking notes.\n",
    "System data is unavailable. Something is wrong underneath.\n",
    "Rockfetch failed. Tyrant demands a better kernel.\n",
    "The specifications remain hidden.\n",
    "System profile failed. Check the machinery.\n",
    "Rockfetch failed. Damian suspects a bad pointer.\n",
    "Hardware information could not be recovered.\n",
    "The machine refused to speak. Tyrant approves of the silence.\n",
    "Rockfetch failed. Something inside the forge went dark.\n",
    "System inspection aborted.\n",
    "Rockfetch failed. Damian is watching the logs.\n",
    "The machine has nothing to report. Tyrant is suspicious.\n",
};

constexpr int ROCKFETCH_FAILURE_PHRASE_COUNT =
    sizeof(rockfetch_failure_phrases) / sizeof(rockfetch_failure_phrases[0]);

/*
 * Hardcoded stats.
 * Replace these with real values later.
 */

const char *stat_labels[] = {
    "Commands executed",     "Successful commands", "Failed commands",
    "Files created",         "Files destroyed",     "Directories created",
    "Directories destroyed", "Bytes written",       "Bytes read",
    "Blocks allocated",      "Blocks freed",        "Inodes allocated",
    "Inodes freed",          "Shell sessions",      "Reboots",
    "Kernel panics",
};

const char *stat_values[] = {
    "143",   "131", "12", "24", "7", "9", "2", "8192",
    "16384", "41",  "17", "31", "8", "6", "4", "0",
};

constexpr int STAT_FIELD_COUNT = sizeof(stat_labels) / sizeof(stat_labels[0]);

/*
 * Statistics command messages.
 */

const char *stat_success_phrases[] = {
    "Statistics retrieved. The machine remembers everything.\n",
    "Stats loaded. Damian has been keeping score.\n",
    "System history recovered successfully.\n",
    "The numbers have spoken. Tyrant approves.\n",
    "Statistics retrieved. Every command left a mark.\n",
    "History loaded. The forge remembers the damage.\n",
    "Stats complete. Damian has seen worse.\n",
    "System activity retrieved successfully.\n",
    "The machine has counted your sins.\n",
    "Statistics loaded. Tyrant is impressed by the destruction.\n",
    "History recovered. Every byte has a story.\n",
    "Stats complete. The machine remembers your work.\n",
    "Activity report loaded. Damian is watching the numbers.\n",
    "System statistics retrieved without incident.\n",
    "The numbers are alive. Tyrant has reviewed them.\n",
    "Command history retrieved. The shell has been busy.\n",
    "Stats loaded. More noise has been officially recorded.\n",
    "Statistics complete. Damian calls that acceptable progress.\n",
    "System history recovered. Nothing important was forgotten.\n",
    "The machine has counted everything. Tyrant remains silent.\n",
};

constexpr int STAT_SUCCESS_PHRASE_COUNT =
    sizeof(stat_success_phrases) / sizeof(stat_success_phrases[0]);

const char *stat_failure_phrases[] = {
    "Statistics unavailable. The machine lost track of itself.\n",
    "Stats failed. The numbers refused to cooperate.\n",
    "Statistics retrieval failed. Damian is checking the logs.\n",
    "The machine cannot remember what happened.\n",
    "Stats unavailable. Something corrupted the history.\n",
    "Statistics failed. Tyrant is demanding answers.\n",
    "The system history could not be recovered.\n",
    "Stats failed. The numbers have disappeared into the void.\n",
    "Statistics unavailable. Damian suspects filesystem trouble.\n",
    "The machine forgot. Tyrant will not be pleased.\n",
    "Stats retrieval failed. The history is silent.\n",
    "System statistics could not be loaded.\n",
    "Statistics failed. Damian has opened another debugger window.\n",
    "The machine cannot account for its actions.\n",
    "Stats failed. Tyrant blames the operator.\n",
    "History retrieval aborted.\n",
    "Statistics unavailable. Something went wrong in the forge.\n",
    "Stats failed. Damian is becoming suspicious.\n",
    "The numbers could not be retrieved.\n",
    "Statistics failed. Tyrant has officially lost patience.\n",
};

constexpr int STAT_FAILURE_PHRASE_COUNT =
    sizeof(stat_failure_phrases) / sizeof(stat_failure_phrases[0]);

/*
 * Hardcoded diagnostic data.
 * Replace these with actual subsystem checks later.
 */

const char *diagnostic_labels[] = {
    "CPU",        "Memory",         "Heap",         "ATA",
    "Interrupts", "Keyboard",       "Timer",        "Filesystem",
    "Superblock", "Inode bitmap",   "Block bitmap", "Inode table",
    "Root inode", "Root directory", "Shell",
};

const char *diagnostic_values[] = {
    "ONLINE", "OK",     "OK",      "ONLINE", "ENABLED",
    "ONLINE", "ONLINE", "MOUNTED", "VALID",  "VALID",
    "VALID",  "VALID",  "VALID",   "VALID",  "READY",
};

constexpr int DIAGNOSTIC_FIELD_COUNT =
    sizeof(diagnostic_labels) / sizeof(diagnostic_labels[0]);

/*
 * Diagnostic command messages.
 */

const char *diagnostic_success_phrases[] = {
    "Diagnostics complete. Every subsystem answered.\n",
    "System diagnostics passed. RockOS stands.\n",
    "All checks passed. Damian can stop worrying.\n",
    "Diagnostics clean. The machine is healthy.\n",
    "Subsystem inspection complete. Tyrant approves.\n",
    "Everything checked out. No fires detected.\n",
    "Diagnostics complete. The forge is running hot.\n",
    "System health verified. Damian has nothing to complain about.\n",
    "All diagnostic checks passed successfully.\n",
    "Tyrant inspected the machine. It survived.\n",
    "Diagnostics complete. Memory, disk, and kernel remain standing.\n",
    "System integrity confirmed. Damian is satisfied.\n",
    "Every subsystem responded. The machine lives.\n",
    "Diagnostics passed. Tyrant calls it acceptable.\n",
    "Health check complete. RockOS is still alive.\n",
    "The machine has survived another inspection.\n",
    "Diagnostics complete. Damian found no obvious disasters.\n",
    "All systems nominal. The forge remains operational.\n",
    "Integrity checks passed. Tyrant has no objections.\n",
    "System diagnostics complete. Maximum stability detected.\n",
};

constexpr int DIAGNOSTIC_SUCCESS_PHRASE_COUNT =
    sizeof(diagnostic_success_phrases) / sizeof(diagnostic_success_phrases[0]);

const char *diagnostic_failure_phrases[] = {
    "Diagnostics failed. Something inside the machine is wrong.\n",
    "System check failed. The machine is hiding something.\n",
    "Diagnostics failed. Damian has entered debugging mode.\n",
    "One or more subsystems refused to cooperate.\n",
    "System integrity check failed.\n",
    "Diagnostics failed. Tyrant is not amused.\n",
    "The machine did not pass inspection.\n",
    "System check failed. Something is burning underneath.\n",
    "Diagnostics failed. Damian has requested the logs.\n",
    "One or more components are reporting trouble.\n",
    "System diagnostics failed. Tyrant demands repairs.\n",
    "The forge has reported a problem.\n",
    "Diagnostics failed. Something has gone very quiet.\n",
    "System health check failed. Damian suspects corruption.\n",
    "The machine is not as healthy as it claims.\n",
    "Diagnostics failed. Tyrant has found a weakness.\n",
    "Integrity check failed. Investigate before proceeding.\n",
    "System inspection failed. Damian is not happy.\n",
    "One or more diagnostic checks did not pass.\n",
    "Diagnostics failed. Tyrant recommends fixing the kernel.\n",
};

constexpr int DIAGNOSTIC_FAILURE_PHRASE_COUNT =
    sizeof(diagnostic_failure_phrases) / sizeof(diagnostic_failure_phrases[0]);

/*
 * File write messages.
 */

const char *write_success_phrases[] = {
    "Written to disk.\n",
    "File updated.\n",
    "Content committed.\n",
    "Bytes hammered into the filesystem.\n",
    "The file has been fed.\n",
    "Write complete. RockOS approves.\n",
    "Disk says: got it.\n",
    "Content successfully committed to the machine.\n",
    "The filesystem has accepted your offering.\n",
    "Written. No sparks detected.\n",
    "Data secured.\n",
    "The file now contains your nonsense.\n",
    "Write successful. Keep making noise.\n",
    "Bytes delivered.\n",
    "The forge has accepted the data.\n",
    "Your words are now on disk.\n",
    "RockFS swallowed the data.\n",
    "Written without incident.\n",
    "The machine remembers.\n",
    "Content successfully carved into the disk.\n",
};

constexpr int WRITE_SUCCESS_PHRASE_COUNT =
    sizeof(write_success_phrases) / sizeof(write_success_phrases[0]);

const char *write_failure_phrases[] = {
    "Write failed.\n",
    "The disk rejected your offering.\n",
    "Could not write to the file.\n",
    "The filesystem said no.\n",
    "Write failed. The machine remains unimpressed.\n",
    "The bytes never made it.\n",
    "RockFS refused the write.\n",
    "Something went wrong while hammering the data into disk.\n",
    "Write failed. Check your path.\n",
    "The file resisted.\n",
    "The disk remained silent.\n",
    "Your data was rejected by the machine.\n",
    "Write failed. Somewhere, a pointer is laughing.\n",
    "The forge rejected the material.\n",
    "Nothing was written.\n",
    "The machine could not commit your data.\n",
    "Write failed. Try again before blaming the kernel.\n",
    "The filesystem has declined your request.\n",
    "Those bytes are going nowhere.\n",
    "Write failed. RockFS stands firm.\n",
};

constexpr int WRITE_FAILURE_PHRASE_COUNT =
    sizeof(write_failure_phrases) / sizeof(write_failure_phrases[0]);

/*
 * Environment command messages.
 */

const char *env_success_phrases[] = {
    "Environment updated. The machine knows something new.\n",
    "Environment variable set. RockOS has taken note.\n",
    "Variable stored. The shell remembers.\n",
    "Environment updated successfully.\n",
    "The shell has accepted the new variable.\n",
    "Variable committed to the environment.\n",
    "Environment modified. Damian approves.\n",
    "The machine has learned something new.\n",
    "Variable set. Tyrant has been informed.\n",
    "Environment updated. No sparks detected.\n",
    "The shell remembers your decision.\n",
    "Variable successfully added to the environment.\n",
    "Environment state changed.\n",
    "The variable has found its place in the shell.\n",
    "Environment updated. Keep building.\n",
    "The shell has recorded the variable.\n",
    "Variable accepted. RockOS carries on.\n",
    "Environment successfully modified.\n",
    "The machine will remember this variable.\n",
    "Environment updated. Maximum configuration achieved.\n",
};

constexpr int ENV_SUCCESS_PHRASE_COUNT =
    sizeof(env_success_phrases) / sizeof(env_success_phrases[0]);

const char *env_failure_phrases[] = {
    "Could not modify the environment.\n",
    "Environment update failed.\n",
    "The shell rejected the variable.\n",
    "Environment operation failed.\n",
    "The machine refused to remember that variable.\n",
    "Environment update failed. Check your arguments.\n",
    "The variable could not be stored.\n",
    "RockOS rejected the environment change.\n",
    "Environment operation failed. Damian is checking the logs.\n",
    "The shell could not process that variable.\n",
    "Variable rejected. Tyrant is not impressed.\n",
    "The environment refused your request.\n",
    "Environment update failed. Something went quiet.\n",
    "The shell couldn't make that change.\n",
    "Variable operation failed. Check the command.\n",
    "Environment modification failed.\n",
    "The machine refused to alter its environment.\n",
    "Environment operation aborted.\n",
    "The variable never made it into the shell.\n",
    "Environment failure. Tyrant blames the operator.\n",
};

constexpr int ENV_FAILURE_PHRASE_COUNT =
    sizeof(env_failure_phrases) / sizeof(env_failure_phrases[0]);

const char *env_get_failure_phrases[] = {
    "That variable does not exist.\n",
    "The shell has never heard of that variable.\n",
    "Variable not found.\n",
    "Nothing in the environment matches that name.\n",
    "The requested variable is missing.\n",
    "RockOS could not find that variable.\n",
    "The machine does not remember that variable.\n",
    "Variable lookup failed. Damian found nothing.\n",
    "Tyrant searched the environment. Nothing.\n",
    "That name isn't in the environment.\n",
};

constexpr int ENV_GET_FAILURE_PHRASE_COUNT =
    sizeof(env_get_failure_phrases) / sizeof(env_get_failure_phrases[0]);

/*
 * Environment command messages.
 */

const char *env_set_success_phrases[] = {
    "Environment variable forged. Damian approves.\n",
    "Variable stored. The machine remembers.\n",
    "Environment updated. Tyrant has accepted the change.\n",
    "Variable successfully carved into the environment.\n",
    "Environment variable set. Keep making noise.\n",
    "Damian has recorded the new variable.\n",
    "The environment has been altered successfully.\n",
    "Variable deployed. Tyrant finds it acceptable.\n",
    "Environment updated without incident.\n",
    "The machine now knows your variable.\n",
};

constexpr int ENV_SET_SUCCESS_PHRASE_COUNT =
    sizeof(env_set_success_phrases) / sizeof(env_set_success_phrases[0]);

const char *env_set_failure_phrases[] = {
    "Environment variable rejected. Damian is suspicious.\n",
    "Could not set the variable. Tyrant blames the operator.\n",
    "Environment update failed. The machine refused.\n",
    "Variable could not be forged.\n",
    "Tyrant rejected the environment change.\n",
    "Environment is full. Damian says make some room.\n",
    "Variable set failed. Something went quiet.\n",
    "The machine refused your variable.\n",
};

constexpr int ENV_SET_FAILURE_PHRASE_COUNT =
    sizeof(env_set_failure_phrases) / sizeof(env_set_failure_phrases[0]);

const char *env_unset_success_phrases[] = {
    "Environment variable destroyed. Damian approves.\n",
    "Variable removed from the machine.\n",
    "Tyrant has erased the variable.\n",
    "Environment cleaned. The machine remembers less now.\n",
    "Variable successfully dismantled.\n",
    "Damian pulled the variable from the environment.\n",
    "Environment variable eliminated.\n",
    "The variable has been banished.\n",
};

constexpr int ENV_UNSET_SUCCESS_PHRASE_COUNT =
    sizeof(env_unset_success_phrases) / sizeof(env_unset_success_phrases[0]);

const char *env_unset_failure_phrases[] = {
    "Variable not found. Damian searched everywhere.\n",
    "Nothing to unset. Tyrant is unimpressed.\n",
    "Environment variable does not exist.\n",
    "The machine could not find that variable.\n",
    "Unset failed. Damian found nothing to destroy.\n",
    "Tyrant searched the environment and found nothing.\n",
};

constexpr int ENV_UNSET_FAILURE_PHRASE_COUNT =
    sizeof(env_unset_failure_phrases) / sizeof(env_unset_failure_phrases[0]);
const char *env_clear_success_phrases[] = {
    "Environment purged. Damian approves the destruction.\n",
    "All environment variables have been wiped.\n",
    "Tyrant has cleared the environment.\n",
    "Environment reset. Nothing remains.\n",
    "The environment has been reduced to silence.\n",
};

constexpr int ENV_CLEAR_SUCCESS_PHRASE_COUNT =
    sizeof(env_clear_success_phrases) / sizeof(env_clear_success_phrases[0]);

const char *date_success_phrases[] = {
    "Damian checked the clock. It says: ",
    "Tyrant consulted the machine. Current time: ",
    "The forge reports the current time: ",
    "Damian says the machine currently reads: ",
    "Tyrant has inspected the clock: ",
    "The machine knows what time it is: ",
    "Clock queried. Damian reports: ",
    "Tyrant says you should probably know the time: ",
    "The clock has spoken: ",
    "Damian checked the RTC. It reports: ",
    "RockOS time signal received: ",
    "The machine has revealed the hour: ",
    "Tyrant has confirmed the current timestamp: ",
    "The RTC is alive and reporting: ",
    "Damian says time keeps moving. Currently: ",
    "Clock status: operational. Current time: ",
    "The forge has checked its clock: ",
    "Tyrant demands that you observe the time: ",
    "RockOS has no idea why you asked, but here it is: ",
    "Time retrieved successfully. Damian approves: ",
};

constexpr int DATE_SUCCESS_PHRASE_COUNT =
    sizeof(date_success_phrases) / sizeof(date_success_phrases[0]);

const char *echo_env_failure_phrases[] = {
    "Damian says that variable doesn't exist.\n",
    "Tyrant says the variable was never forged.\n",
    "RockOS couldn't find that environment variable.\n",
    "Damian searched the environment. Nothing there.\n",
    "Tyrant says: unknown variable.\n",
    "That variable has vanished into the void.\n",
    "The forge contains no such variable.\n",
    "Damian couldn't find what you were looking for.\n",
    "Tyrant checked the environment. Empty.\n",
    "Unknown environment variable. Damian is confused.\n",
    "RockOS searched everywhere. The variable isn't here.\n",
    "The machine has no memory of that variable.\n",
    "Damian says you should set it first.\n",
    "Tyrant says: define your variables properly.\n",
    "No such variable exists in the environment.\n",
};

constexpr int ECHO_ENV_FAILURE_PHRASE_COUNT =
    sizeof(echo_env_failure_phrases) / sizeof(echo_env_failure_phrases[0]);

const char *foolish_phrases[] = {
    "Wrong command. Try using your brain next time.\n",
    "Damian says that was a stupid command.\n",
    "Tyrant rejects your pathetic attempt at syntax.\n",
    "You have somehow managed to confuse the shell.\n",
    "That command is garbage. Fix it.\n",
    "Damian is disappointed. Deeply disappointed.\n",
    "Tyrant says: learn the damn syntax.\n",
    "Invalid arguments. Even the machine knows you screwed up.\n",
    "What the hell was that supposed to mean?\n",
    "RockOS cannot interpret your nonsense.\n",
    "Damian has reviewed your command and found it embarrassing.\n",
    "Tyrant says you are wasting CPU cycles.\n",
    "Wrong arguments. Stop throwing random shit at the shell.\n",
    "That is not how this command works, genius.\n",
    "Damian says: read the damn help page.\n",
    "Tyrant refuses to process this garbage.\n",
    "Your syntax is broken. Just like your judgment.\n",
    "Command rejected. Tyrant has no patience for this bullshit.\n",
    "Damian is staring at the command in disbelief.\n",
    "The shell expected arguments. You provided stupidity.\n",
    "Invalid syntax. Try again without embarrassing yourself.\n",
    "Tyrant says: insufficient competence detected.\n",
    "That command makes absolutely no fucking sense.\n",
    "Damian says you forgot something. Probably several things.\n",
    "Arguments missing. The machine cannot read your mind.\n",
    "Tyrant has classified this command as worthless.\n",
    "Wrong usage. You had one job.\n",
    "RockOS has rejected your linguistic assault on the shell.\n",
    "Damian says: that syntax is fucking atrocious.\n",
    "Tyrant says the command is malformed garbage.\n",
    "You entered the command incorrectly. Impressive.\n",
    "Invalid arguments. Even the filesystem is judging you.\n",
    "Damian recommends consulting the help command before trying again.\n",
    "Tyrant recommends developing basic command-line literacy.\n",
    "The machine understands commands. It does not understand whatever that "
    "was.\n",
    "Wrong number of arguments. Count them next time.\n",
    "Damian says your command has fallen apart spectacularly.\n",
    "Tyrant says: syntax first, chaos later.\n",
    "Command rejected. Your input is malformed beyond recognition.\n",
    "That argument does not belong there, idiot.\n",
    "Damian has found a new way for you to misuse the shell.\n",
    "Tyrant is running out of patience.\n",
    "Invalid usage. Stop stabbing the keyboard and read the documentation.\n",
    "The shell is not responsible for your inability to count arguments.\n",
    "Damian says: try `help` before making things worse.\n",
    "Tyrant says your command is a disgrace to command-line interfaces.\n",
    "Bad syntax detected. Intelligence not detected.\n",
    "Your command is wrong. The machine is not.\n",
    "Damian says the problem is somewhere between the keyboard and the "
    "chair.\n",
    "Tyrant says the operator has failed another basic test.\n",
    "Invalid command structure. Try using actual syntax.\n",
    "That argument is bullshit and the shell knows it.\n",
    "Damian refuses to dignify that command with execution.\n",
    "Tyrant has rejected your offering.\n",
    "You supplied the wrong arguments. Congratulations.\n",
    "The shell asked for something useful. You gave it garbage.\n",
    "Damian says: stop guessing.\n",
    "Tyrant says: stop fucking guessing.\n",
    "Invalid input. The machine has standards.\n",
    "Your command has been denied on grounds of incompetence.\n",
    "Damian found nothing useful in your command.\n",
    "Tyrant found exactly one thing: failure.\n",
    "Wrong usage. Read the command description, you absolute menace.\n",
    "The command requires arguments. This is not a suggestion.\n",
    "Damian says you forgot the important part.\n",
    "Tyrant says you forgot how commands work.\n",
    "That syntax is so wrong it offended the parser.\n",
    "RockOS rejects this command on principle.\n",
    "Damian says: impressive. Somehow you broke basic syntax.\n",
    "Tyrant says: do it properly or don't do it at all.\n",
    "Invalid arguments. The shell is not your fucking guessing game.\n",
    "Wrong command. Try again before Damian loses his patience.\n",
    "Tyrant has no mercy for malformed commands.\n",
    "The parser has suffered enough.\n",
    "Damian says your command needs serious help.\n",
    "Tyrant says your command needs serious fucking help.\n",
    "Command rejected. Syntax was murdered on arrival.\n",
    "Your arguments are in the wrong place. Fix your mess.\n",
    "Damian says: perhaps read the usage message next time.\n",
    "Tyrant says: perhaps learn what an argument is.\n",
    "Invalid usage. Even a drunk shell could do better.\n",
    "The machine is powerful. Your command is not.\n",
    "Damian expected competence. He received this.\n",
    "Tyrant expected obedience. He received garbage.\n",
    "Wrong syntax. The shell refuses to participate in this stupidity.\n",
    "You have successfully entered a command that makes no sense.\n",
    "Damian says: what the hell are you doing?\n",
    "Tyrant says: what the fuck are you doing?\n",
    "Invalid arguments. Try reading before typing.\n",
    "The command was rejected because you used it incorrectly.\n",
    "Damian has identified operator error.\n",
    "Tyrant has identified operator incompetence.\n",
    "Bad arguments. Good luck blaming the kernel for this one.\n",
    "Your command failed before it even had a chance.\n",
    "RockOS is not psychic. Provide the required arguments.\n",
    "Damian says the syntax is not optional.\n",
    "Tyrant says the syntax is law.\n",
    "You cannot simply throw words at the shell and hope.\n",
    "That command is malformed. Fix it, asshole.\n",
    "Damian says: stop making the machine suffer.\n",
    "Tyrant says: stop wasting my fucking time.\n",
    "Invalid usage. The machine has better things to do.\n",
    "You fucked up the arguments. Try again.\n",
    "Command rejected. Tyrant has spoken.\n",
    "Damian says: syntax error. Human error.\n",
    "Tyrant says: syntax error. Operator error.\n",
    "The shell understands structure. You provided chaos.\n",
    "Wrong number of arguments. This is basic shit.\n",
    "Damian says even the help command could explain this to you.\n",
    "Tyrant says even the help command is disappointed in you.\n",
    "Invalid input. Please stop assaulting the command line.\n",
    "That was not a command. That was keyboard abuse.\n",
    "Damian has seen better commands from broken keyboards.\n",
    "Tyrant has seen better syntax from corrupted disks.\n",
    "Wrong arguments. The shell refuses your nonsense.\n",
    "You have angered the parser.\n",
    "Damian says: congratulations, you confused a computer.\n",
    "Tyrant says: congratulations, you wasted a perfectly good CPU cycle.\n",
    "The command is wrong. The universe will continue without it.\n",
    "Invalid usage. Try again, preferably with thought this time.\n",
    "Damian says: use your fucking brain.\n",
    "Tyrant says: use your brain before using the keyboard.\n",
    "Bad syntax detected. RockOS remains unimpressed.\n",
    "Your command has been judged and found wanting.\n",
    "Tyrant has judged your command. It has failed.\n",
    "Damian has judged your syntax. It is ugly.\n",
    "Wrong arguments. Clean up your mess and try again.\n",
    "The shell refuses to execute your malformed nonsense.\n",
    "Damian says: this is why we have documentation.\n",
    "Tyrant says: this is why we have documentation, idiot.\n",
    "Invalid command. Try something that actually exists.\n",
    "That argument is not recognized. Neither is your reasoning.\n",
    "The parser expected competence and received chaos.\n",
    "Damian says the command is fucked.\n",
    "Tyrant says the command is fucking fucked.\n",
    "Wrong usage. You have disappointed the machine once again.\n",
    "The shell has rejected your stupidity with extreme prejudice.\n",
    "Damian says: fix it.\n",
    "Tyrant says: fix it or stop typing.\n",
    "Invalid arguments. End of discussion.\n",
    "The machine has spoken: no.\n",
    "Damian says: absolutely fucking not.\n",
    "Tyrant says: denied. Try again when you understand the command.\n",
};

constexpr int FOOLISH_PHRASE_COUNT =
    sizeof(foolish_phrases) / sizeof(foolish_phrases[0]);
