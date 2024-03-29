#include <iostream>
#include <cstdlib>
#include <memory>

#include "backend/motherboard.h"

// Include your frontends implementations
#include "frontend/display.h"

static const char *usage =
R"(Usage: skygameboy <game ROM>
    - game ROM: path to a Gameboy game to launch)";

static std::string parseCLI(int argc, char **argv)
{
    if (argc <= 1)
    {
        std::cerr << usage << std::endl;

        std::exit(1);
    }

    return std::string(argv[1]);
}

int main(int argc, char **argv)
{
    const std::string gameROM = parseCLI(argc, argv);
    const std::string biosROM = "./roms/dmg_boot.bin";

    // Initialize here all your frontends
    // They must have same lifetime as the application
    Display display;

    Motherboard motherboard(biosROM, gameROM, display, display);
    motherboard.run();

    return 0;
}
