#pragma once

const char *ascii_art[] = {

    R"ASCII(
 ____          _      ___  ____  
|  _ \ ___   ___| | __/ _ \/ ___| 
| |_) / _ \ / __| |/ / | | \___ \ 
|  _ < (_) | (__|   <| |_| |___) |
|_| \_\___/ \___|_|\_\\___/|____/ 
)ASCII",
    /*
     * THESE ARE UNICODE CHARACTERS, UNICODE REQUIRE
     * MORE HANDLING WHICH IS NOT IMPLEMENTED RIGHT NOW
    R"ASCII(
     ██████╗  ██████╗  ██████╗██╗  ██╗ ██████╗ ███████╗
     ██╔══██╗██╔═══██╗██╔════╝██║ ██╔╝██╔═══██╗██╔════╝
     ██████╔╝██║   ██║██║     █████╔╝ ██║   ██║███████╗
     ██╔══██╗██║   ██║██║     ██╔═██╗ ██║   ██║╚════██║
     ██║  ██║╚██████╔╝╚██████╗██║  ██╗╚██████╔╝███████║
     ╚═╝  ╚═╝ ╚═════╝  ╚═════╝╚═╝  ╚═╝ ╚═════╝ ╚══════╝
    )ASCII",
    */
    R"ASCII(
        .        .
       / \      / \
      /   \____/   \
     |    ROCKOS    |
     |      /\      |
     |     /  \     |
     |____/____\____|
         ||    ||
         ||____||
)ASCII",

    R"ASCII(
        ______________________
       /                      \
      /      R O C K O S       \
     |                          |
     |    KERNEL // ONLINE      |
     |    FILESYSTEM // READY   |
     |    SHELL // WAITING      |
     |__________________________|
            ||      ||
            ||      ||
         ___||______||___
        /_______________\
)ASCII",

    R"ASCII(
             _____
            /     \
           /  VOID \
          |         |
          |    ?    |
          |         |
           \       /
            \_____/
              ||
              ||
         THE VOID WATCHES
)ASCII",

    R"ASCII(
    _________________________
   /                         \
  |  [SYSTEM] ROCKOS v1.0    |
  |  ---------------------   |
  |  > Loading kernel... OK  |
  |  > Mounting root...  OK  |
  |  > Spawning init...  OK  |
   \_________________________/
)ASCII",

    R"ASCII(
         .---.
        /     \
       |  ROK  |
       |  SEC  |
        \     /
         `---'
      _.-'   `-._
    .'   ROCKOS   `.
   /   SECURE CORE  \
  +------------------+
)ASCII",

    R"ASCII(
       +--------------------+
       | [o] [o] [o]  ROCKOS|
       |--------------------|
       | [o] [o] [o]  ROCKOS|
       |--------------------|
       | [o] [o] [o]  ROCKOS|
       |--------------------|
       | [ ] [ ] [ ]  IDLE  |
       +--------------------+
         ||||||||||||||||  
       ======================
)ASCII",

};

constexpr int ASCII_ART_COUNT = sizeof(ascii_art) / sizeof(ascii_art[0]);
