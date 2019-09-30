#include "lcd.h"
#include "virtual_memory.h"

LCD::LCD(VirtualMemory &memory, IDisplay &display) : memory(memory), display(display)
{

}

void LCD::cycles(uint16 elapsedCycles)
{
    currentElapsedCycles += elapsedCycles;

    // TODO: GPU timing should not be constant
    // HBLANK ended
    if (currentMode == Mode::HBLANK && currentElapsedCycles >= 204)
    {
        currentElapsedCycles = 0;
        if (memory.LY >= 143)
        {
            // start vblank
            memory.STAT = (memory.STAT & ~memory.STATBits.currentMode) | memory.STATBits.currentModeVBlank;
            currentMode = Mode::VBLANK;
            display.newFrameIsReady(buffer);

            updateSTATIRQ();
            memory.interruptRequest |= memory.interruptBits.verticalBlank;
        }
        else
        {
            // start OAM search
            memory.STAT = (memory.STAT & ~memory.STATBits.currentMode) | memory.STATBits.currentModeOAM;
            currentMode = Mode::OAM;
            incrementLY();

            updateSTATIRQ();
        }
    }
    // OAM read ended
    else if (currentMode == Mode::OAM && currentElapsedCycles >= 80)
    {
        // start OAM and VRAM transfer
        currentElapsedCycles = 0;
        currentMode = Mode::Transfer;
        memory.STAT = (memory.STAT & ~memory.STATBits.currentMode) | memory.STATBits.currentModeDataTransfer;
    }
    // OAM and VRAM read ended
    else if (currentMode == Mode::Transfer && currentElapsedCycles >= 172)
    {
        // Draw a line and start HBLANK
        currentElapsedCycles = 0;
        currentMode = Mode::HBLANK;
        memory.STAT = (memory.STAT & ~memory.STATBits.currentMode) | memory.STATBits.currentModeHBlank;
        drawLine();

        updateSTATIRQ();
    }
    // VBLANK ended a line
    else if (currentMode == Mode::VBLANK && currentElapsedCycles >= 456)
    {
        currentElapsedCycles = 0;
        incrementLY();

        if (memory.LY > 153)
        {
            // VBLANK ended all lines
            // OAM search start
            currentMode = Mode::OAM;
            memory.STAT = (memory.STAT & ~memory.STATBits.currentMode) | memory.STATBits.currentModeOAM;
            setLY(0);

            updateSTATIRQ();
        }
    }
}

void LCD::drawLine()
{
//    const struct
//    {
//        const uint8 x = 32;
//        const uint8 y = 32;
//    } backgroundTileSize;
//
//    const struct
//    {
//        const uint8 x = 8;
//        const uint8 y = 8;
//    } tileSize;

    const uint16 backgroundTilemapAddr = memory.lcdControl & memory.lcdControlBits.backgroundTilemap ? 0x9C00 : 0x9800;
    const uint16 tilesetAddr = memory.lcdControl & memory.lcdControlBits.tileset ? 0x8000 : 0x8800;
    const bool tilemapSigned = tilesetAddr == 0x8800;
    const uint8 scrollX = memory.scrollX;
    const uint8 scrollY = memory.scrollY;
    const size_t screenY = memory.LY;
    const uint8 bgY = (screenY + scrollY);// % HEIGHT;

    for (size_t screenX = 0; screenX < WIDTH; ++screenX)
    {
        const uint8 bgX = (scrollX + screenX);// % WIDTH;

        if (memory.lcdControl & memory.lcdControlBits.backgroundEnable)
        {
            // From current screen (X,Y), get address of current tilemap
            const uint16 currentTilemapIndex = (bgX / 8u) + ((bgY / 8u) * 32);
            const uint16 currentTilemapAddr = backgroundTilemapAddr + currentTilemapIndex;

            // Get current id of the tile being drawn
            int16 currentTilesetId = 0;
            if (tilemapSigned)
                currentTilesetId = static_cast<int8>(memory.read8(currentTilemapAddr));
            else
                currentTilesetId = memory.read8(currentTilemapAddr);

            // Get address to read that tile
            const uint16 currentTilesetAddr = (currentTilesetId * 16) + tilesetAddr;

            // Get position inside the tile
            const uint8 tilePosX = 7 - (bgX % 8u); // says which bit to read
            const uint8 tilePosY = bgY % 8u * 2; // says which bytes (two bytes per line) to read

            // address of both bytes for the current line of the tile
            const uint8 tile0 = memory.read8(currentTilesetAddr + tilePosY + 0);
            const uint8 tile1 = memory.read8(currentTilesetAddr + tilePosY + 1);

            // actual DMG color of the pixel [0;3]
            const uint8 color = ((tile0 >> tilePosX) & 1u) + (((tile1 >> tilePosX) & 1u) << 1);

            // Get the SFML color
            std::array<uint8, 3> SFMLColor = colors[color];

            // print it in the final buffer
            buffer[(screenX + (screenY * WIDTH)) * 3 + 0] = SFMLColor[0];
            buffer[(screenX + (screenY * WIDTH)) * 3 + 1] = SFMLColor[1];
            buffer[(screenX + (screenY * WIDTH)) * 3 + 2] = SFMLColor[2];
        }
    }
}

void LCD::incrementLY()
{
    ++memory.LY;
    updateLY();
}

void LCD::setLY(uint8 value)
{
    memory.LY = value;
    updateLY();
}

void LCD::updateLY()
{
    if (memory.LY == memory.LYC)
    {
        memory.STAT |= memory.STATBits.LYCEquality;
    }
    else
    {
        memory.STAT &= ~memory.STATBits.LYCEquality;
    }
}

void LCD::updateSTATIRQ()
{
    bool newSignalStatus = false;

    if (memory.LY == memory.LYC && (memory.STAT & memory.STATBits.LYCInterruptEnable))
    {
        newSignalStatus = true;
    }

    if (currentMode == Mode::HBLANK && (memory.STAT & memory.STATBits.HBlankInterruptEnable))
    {
        newSignalStatus = true;
    }

    if (currentMode == Mode::OAM && (memory.STAT & memory.STATBits.OAMInterruptEnable))
    {
        newSignalStatus = true;
    }

    // According to cycle accurate gameboy guide, it OAM interrupt enable also trigger it
    if (currentMode == Mode::VBLANK && (memory.STAT & (memory.STATBits.VBlankInterruptEnable | memory.STATBits.OAMInterruptEnable)))
    {
        newSignalStatus = true;
    }

    // If signal goes from low to high
    if (!STATIRQSignal && newSignalStatus)
    {
        memory.interruptRequest |= memory.interruptBits.STAT;
    }
    STATIRQSignal = newSignalStatus;
}
