#ifndef FRACTAL_LCD_H
#define FRACTAL_LCD_H

#include <array>
#include <vector>

#include "virtual_memory.h"
#include "../frontend/interfaces/i_display.h"

/**
 * Imagine LCD is a standalone chip in the Gameboy, being able to access
 * memory (which explain VirtualMemory reference) and does its work autonomously.
 *
 * It is therefore a standalone running class which is updated according to
 * elapsed cycles.
 *
 * All VRAM and video registers are still managed by VirtualMemory (because they are
 * still accessible by CPU).
 *
 * Display is 160*144 pixels.
 *
 * LCD begin in Mode 0 (Horizontal blank). Therfore wait for HBLANK then draw first line.
 *
 * - Mode 2 OAM read last 77-83 cycles     <-----|
 * - Mode 3 OAM and VRAM read 169-175 cycles     |
 * - Mode 0 HBLANK last 201-207 cycles     >-----|
 * - Mode 1 VBLANK last 4560 cycles
 *
 * A loop from Mode 2 to Mode 0 last 456 cycles.
 *
 * VBLANK draw 10 more lines out of screen (456 * 10) but it is for timing. It does nothing.
 * A complete screen draw (including VBLANK) last 70224 cycles (456*154).
 * At the processor speed, it make the screen almost 60hz.
 */
class LCD
{
public:
    explicit LCD(VirtualMemory &memory, IDisplay &display);

    /**
     * Give a new amount of elapsed cycles.
     */
    void cycles(uint16 elapsedCycles);

    static const constexpr std::array<std::array<uint8, 3>, 4> colors =
    {{
        {{255, 255, 255}},
        {{192, 192, 192}},
        {{96, 96, 96}},
        {{0, 0, 0}}
    }};

private:
    static const constexpr size_t WIDTH = 160;
    static const constexpr size_t HEIGHT = 144;

    VirtualMemory &memory;
public:
    IDisplay &display;
private:

    enum class Mode
    {
        HBLANK, // Mode 0
        VBLANK, // Mode 1
        OAM, // Mode 2
        Transfer // Mode 3
    };
    Mode currentMode = Mode::HBLANK;

    // Cycles elapsed for current mode
    uint16 currentElapsedCycles = 0;

    std::vector<uint8> buffer = std::vector<uint8>(WIDTH * HEIGHT * 3, 0);

    void drawLine();
    void incrementLY();
    void setLY(uint8 value);
    void updateLY();

    bool STATIRQSignal = false;
    void updateSTATIRQ();
};


#endif //FRACTAL_LCD_H
