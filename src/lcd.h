#ifndef FRACTAL_LCD_H
#define FRACTAL_LCD_H

#include "virtual_memory.h"

/**
 * Imagine LCD is a standalone chip in the Gameboy, being able to access
 * memory (which explain VirtualMemory pointer) and does it work autonomously
 * in concordance with CPU cycles.
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
    explicit LCD(VirtualMemory *memory);

    /**
     * Give a new amount of elapsed cycles.
     */
    void cycles(uint16 elapsedCycles);

private:
    // it is owned by CPU
    const VirtualMemory *memory;

    enum class Mode
    {
        Mode0 = 0, // hblank
        Mode1, // VBLANK
        Mode2, // OAM read
        Mode3 // OAM and VRAM read
    };
    Mode currentMode = Mode::Mode0;

    // Cycles elapsed for current mode
    uint16 currentElapsedCycles = 0;

    void drawLine();
};


#endif //FRACTAL_LCD_H