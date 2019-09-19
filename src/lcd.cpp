#include "lcd.h"

LCD::LCD(VirtualMemory *memory) : memory(memory)
{

}

void LCD::cycles(uint16 elapsedCycles)
{
    currentElapsedCycles += elapsedCycles;

    // TODO: GPU timing are not constant
    if (currentMode == Mode::Mode0 && currentElapsedCycles == 204)
    {
        currentElapsedCycles = 0;
        if (memory->LY == 144)
        {
            currentMode = Mode::Mode1;
        }
        else
        {
            currentMode = Mode::Mode2;
        }
    }
    else if (currentMode == Mode::Mode2 && currentElapsedCycles == 80)
    {
        currentMode = Mode::Mode3;
        currentElapsedCycles = 0;
    }
    else if (currentMode == Mode::Mode3 && currentElapsedCycles == 172)
    {
        currentMode = Mode::Mode0;
        currentElapsedCycles = 0;
    }
}
