#include "lcd.h"
#include "virtual_memory.h"

LCD::LCD(VirtualMemory *memory) : memory(memory)
{

}

void LCD::cycles(uint16 elapsedCycles)
{
    currentElapsedCycles += elapsedCycles;

    // TODO: GPU timing should not be constant
    if (currentMode == Mode::Mode0 && currentElapsedCycles >= 204)
    {
        currentElapsedCycles = 0;
        if (memory->LY >= 143)
        {
            memory->STAT = (memory->STAT & ~memory->STATBits.currentMode) | 1u;
            currentMode = Mode::Mode1;
            display.pollEvents();
            display.print(buffer);

            if (memory->STAT & memory->STATBits.mode1Enable)
            {
                memory->interruptRequest |= memory->interruptBits.lcdStat;
            }
            memory->interruptRequest |= memory->interruptBits.verticalBlank;
        }
        else
        {
            memory->STAT = (memory->STAT & ~memory->STATBits.currentMode) | 2u;
            ++memory->LY;
            currentMode = Mode::Mode2;

            if (memory->STAT & memory->STATBits.mode2Enable)
            {
                memory->interruptRequest |= memory->interruptBits.lcdStat;
            }
        }
    }
    else if (currentMode == Mode::Mode2 && currentElapsedCycles >= 80)
    {
        currentMode = Mode::Mode3;
        currentElapsedCycles = 0;
        memory->STAT = (memory->STAT & ~memory->STATBits.currentMode) | 3u;
    }
    else if (currentMode == Mode::Mode3 && currentElapsedCycles >= 172)
    {
        drawLine();
        currentMode = Mode::Mode0;
        currentElapsedCycles = 0;
        memory->STAT = (memory->STAT & ~memory->STATBits.currentMode) | 0u;

        if (memory->STAT & memory->STATBits.mode0Enable)
        {
            memory->interruptRequest |= memory->interruptBits.lcdStat;
        }
    }
    else if (currentMode == Mode::Mode1 && currentElapsedCycles >= 456)
    {
        currentElapsedCycles = 0;
        ++memory->LY;

        if (memory->LY > 153)
        {
            currentMode = Mode::Mode2;
            memory->STAT = (memory->STAT & ~memory->STATBits.currentMode) | 2u;
            memory->LY = 0;

            if (memory->STAT & memory->STATBits.mode2Enable)
            {
                memory->interruptRequest |= memory->interruptBits.lcdStat;
            }
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

    const uint16 backgroundTilemapAddr = memory->lcdControl & memory->lcdControlBits.backgroundTilemap ? 0x9C00 : 0x9800;
    const uint16 tilesetAddr = memory->lcdControl & memory->lcdControlBits.tileset ? 0x8000 : 0x8800;
    const bool tilemapSigned = tilesetAddr == 0x8800;
    const uint8 scrollX = memory->scrollX;
    const uint8 scrollY = memory->scrollY;
    const size_t screenY = memory->LY;
    const uint8 bgY = (screenY + scrollY);// % HEIGHT;

    for (size_t screenX = 0; screenX < WIDTH; ++screenX)
    {
        const uint8 bgX = (scrollX + screenX);// % WIDTH;

        if (memory->lcdControl & memory->lcdControlBits.backgroundEnable)
        {
            // From current screen (X,Y), get address of current tilemap
            const uint16 currentTilemapIndex = (bgX / 8u) + ((bgY / 8u) * 32);
            const uint16 currentTilemapAddr = backgroundTilemapAddr + currentTilemapIndex;

            // Get current id of the tile being drawn
            int16 currentTilesetId = 0;
            if (tilemapSigned)
                currentTilesetId = static_cast<int8>(memory->read8(currentTilemapAddr));
            else
                currentTilesetId = memory->read8(currentTilemapAddr);

            // Get address to read that tile
            const uint16 currentTilesetAddr = (currentTilesetId * 16) + tilesetAddr;

            // Get position inside the tile
            const uint8 tilePosX = 7 - (bgX % 8u); // says which bit to read
            const uint8 tilePosY = bgY % 8u * 2; // says which bytes (two bytes per line) to read

            // address of both bytes for the current line of the tile
            const uint8 tile0 = memory->read8(currentTilesetAddr + tilePosY + 0);
            const uint8 tile1 = memory->read8(currentTilesetAddr + tilePosY + 1);

            // actual DMG color of the pixel [0;3]
            const uint8 color = ((tile0 >> tilePosX) & 1u) + (((tile1 >> tilePosX) & 1u) << 1);

            // Get the SFML color
            std::array<uint8, 4> SFMLColor = colors[color];

            // print it in the final buffer
            buffer[(screenX + (screenY * WIDTH)) * 4 + 0] = SFMLColor[0];
            buffer[(screenX + (screenY * WIDTH)) * 4 + 1] = SFMLColor[1];
            buffer[(screenX + (screenY * WIDTH)) * 4 + 2] = SFMLColor[2];
            buffer[(screenX + (screenY * WIDTH)) * 4 + 3] = SFMLColor[3];
        }
    }
}
