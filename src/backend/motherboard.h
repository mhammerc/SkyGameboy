#ifndef FRACTAL_MOTHERBOARD_H
#define FRACTAL_MOTHERBOARD_H

#include <string>

#include "../frontend/interfaces/i_display.h"
#include "cpu/cpu.h"
#include "virtual_memory.h"
#include "lcd.h"

/**
 * This class represent the Gameboy "motherboard": it hold CPU, and all
 * components required to run the software.
 *
 * Components need other components to work. They are passed by reference.
 * As soon as Motherboard destruct, all components will destruct. Passing reference of component
 * to components internal of Motherboard is no danger.
 */
class Motherboard
{
public:
    explicit Motherboard(const std::string &biosRomPath, const std::string &gameRomPath, IDisplay &display):
    memory(biosRomPath, gameRomPath), lcd(memory, display), cpu(memory, lcd)
    {

    };

    VirtualMemory memory;
    LCD lcd;
    CPU cpu;

    /**
     * Run the software until it stop.
     */
    void run()
    {
        while (true)
        {
            cpu.nextTick();
        }
    };
};

#endif //FRACTAL_MOTHERBOARD_H
