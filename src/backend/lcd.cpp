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
    drawBackground();
    drawWindow();
    drawSprites();
}

void LCD::drawBackground()
{
    if (!(memory.lcdControl & memory.lcdControlBits.backgroundEnable))
    {
        return;
    }

    const uint16 backgroundTilemapAddr = memory.lcdControl & memory.lcdControlBits.backgroundTilemap ? 0x9C00 : 0x9800;
    const uint16 tilesetAddr = memory.lcdControl & memory.lcdControlBits.tileset ? 0x8000 : 0x8800;
    const bool tilemapSigned = tilesetAddr == 0x8800;
    const uint8 palette = memory.backgroundPalette;
    const uint8 scrollX = memory.scrollX;
    const uint8 scrollY = memory.scrollY;
    const size_t screenY = memory.LY;
    const uint8 bgY = (screenY + scrollY) % 256;

    for (size_t screenX = 0; screenX < SCREEN_WIDTH; ++screenX)
    {
        const uint8 bgX = (scrollX + screenX) % 256;

        // From current screen (X,Y), get address of current tilemap
        const uint16 currentTilemapIndex = (bgX / 8u) + ((bgY / 8u) * 32u);
        const uint16 currentTilemapAddr = backgroundTilemapAddr + currentTilemapIndex;

        // Get current id of the tile being drawn
        int16 currentTilesetId = 0;
        if (tilemapSigned)
            currentTilesetId = static_cast<int8>(memory.read8(currentTilemapAddr)) + 128;
        else
            currentTilesetId = memory.read8(currentTilemapAddr);

        // Get address to read that tile
        const uint16 currentTilesetAddr = tilesetAddr + (currentTilesetId * 16);

        // Get position inside the tile
        const uint16 tilePosX = 7 - (bgX % 8u); // says which bit to read
        const uint16 tilePosY = bgY % 8u * 2; // says which bytes (two bytes per line) to read

        // address of both bytes for the current line of the tile
        const uint8 tile0 = memory.read8(currentTilesetAddr + tilePosY + 0u);
        const uint8 tile1 = memory.read8(currentTilesetAddr + tilePosY + 1u);

        // actual DMG color of the pixel [0;3]
        const uint8 color = ((tile0 >> tilePosX) & 1u) + (((tile1 >> tilePosX) & 1u) << 1u);
        const uint8 paletteColor = (palette >> (color * 2u) & 1u) + ((palette >> (color * 2u + 1) & 1u) << 1u);

        // Get the SFML color
        std::array<uint8, 3> SFMLColor = colors[paletteColor];

        // print it in the final buffer
        buffer[(screenX + (screenY * SCREEN_WIDTH)) * 3 + 0] = SFMLColor[0];
        buffer[(screenX + (screenY * SCREEN_WIDTH)) * 3 + 1] = SFMLColor[1];
        buffer[(screenX + (screenY * SCREEN_WIDTH)) * 3 + 2] = SFMLColor[2];
    }
}

void LCD::drawWindow()
{
    if (!(memory.lcdControl & memory.lcdControlBits.windowEnable))
    {
        return;
    }

    const uint16 tilemapAddr = memory.lcdControl & memory.lcdControlBits.windowTilemap ? 0x9C00 : 0x9800;
    const uint16 tilesetAddr = memory.lcdControl & memory.lcdControlBits.tileset ? 0x8000 : 0x8800;
    const bool tilemapSigned = tilesetAddr == 0x8800;
    const uint8 palette = memory.backgroundPalette;
    const uint8 WX = memory.windowX;
    const uint8 WY = memory.windowY;
    Vector2i screenPosition {0, memory.LY};

    // There is no window on this line.
    if (WY > screenPosition.y)
    {
        return;
    }

    for (screenPosition.x = 0; screenPosition.x < SCREEN_WIDTH; ++screenPosition.x)
    {
        const Vector2i windowPosition {screenPosition.x - (WX - 7), screenPosition.y - WY};
        if (windowPosition.x < 0 || windowPosition.x > SCREEN_WIDTH)
        {
            continue;
        }

        // From current screen (X,Y), get address of current tilemap
        const uint16 currentTilemapIndex = (windowPosition.x / 8u) + ((windowPosition.y / 8u) * 32u);
        const uint16 currentTilemapAddr = tilemapAddr + currentTilemapIndex;

        // Get current id of the tile being drawn
        int16 currentTilesetId = 0;
        if (tilemapSigned)
            currentTilesetId = static_cast<int8>(memory.read8(currentTilemapAddr)) + 128;
        else
            currentTilesetId = memory.read8(currentTilemapAddr);

        // Get address to read that tile
        const uint16 currentTilesetAddr = tilesetAddr + (currentTilesetId * 16);

        const uint16 tilePosX = 7 - (windowPosition.x % 8u); // says which bit to read
        const uint16 tilePosY = windowPosition.y % 8u * 2; // says which bytes (two bytes per line) to read

        // address of both bytes for the current line of the tile
        const uint8 tile0 = memory.read8(currentTilesetAddr + tilePosY + 0u);
        const uint8 tile1 = memory.read8(currentTilesetAddr + tilePosY + 1u);

        // actual DMG color of the pixel [0;3]
        const uint8 color = ((tile0 >> tilePosX) & 1u) + (((tile1 >> tilePosX) & 1u) << 1u);
        const uint8 paletteColor = (palette >> (color * 2u) & 1u) + ((palette >> (color * 2u + 1) & 1u) << 1u);

        // Get the SFML color
        std::array<uint8, 3> SFMLColor = colors[paletteColor];

        // print it in the final buffer
        buffer[(screenPosition.x + (screenPosition.y * SCREEN_WIDTH)) * 3 + 0] = SFMLColor[0];
        buffer[(screenPosition.x + (screenPosition.y * SCREEN_WIDTH)) * 3 + 1] = SFMLColor[1];
        buffer[(screenPosition.x + (screenPosition.y * SCREEN_WIDTH)) * 3 + 2] = SFMLColor[2];
    }
}

void LCD::drawSprites()
{
    if (!(memory.lcdControl & memory.lcdControlBits.spritesEnable))
    {
        return;
    }

    // Up to 40 sprites readable
    std::array<SpriteAttribute, 40> sprites {};
    memcpy(sprites.data(), memory.oamRAM.data(), sizeof(SpriteAttribute) * 40);

    // Draw a max of 10 sprites
    std::vector<SpriteAttribute> spritesToDraw {};
    spritesToDraw.reserve(10);

    // Are sprite 8x16 (true) or 8x8 (false)? If true, LSB of `tilesetId` is ignored
    const bool areSpritesBig = memory.lcdControl & memory.lcdControlBits.spriteSize;
    Vector2i spriteSize = {8, 8};
    const uint8 bytesPerSprite = areSpritesBig ? 32 : 16;
    spriteSize.y = areSpritesBig ? 16 : 8;

    // Go through each of the 40 sprites and find which sprite will be drawn. Max of 10 sprites.
    // When sprites overlap with same X coordinates, firsts sprites in OAM table have priority (sprites[0] have priority over sprites[1])
    for (auto it = sprites.cbegin(); it != sprites.cend() && spritesToDraw.size() <= 10; ++it)
    {
        const SpriteAttribute &sprite = *it;
        Vector2i screenPosition { sprite.x - 8, sprite.y - 16};

        // Does it fit on screen for current line?
        if (screenPosition.y <= memory.LY && screenPosition.y + spriteSize.y > memory.LY
            && screenPosition.x >= -7 && screenPosition.x < SCREEN_WIDTH)
        {
            spritesToDraw.push_back(sprite);
        }
    }

    // Smaller X coordinate sprites have higher priority. Meanwhile, on same X coordinate, first sprite is priority.
    // Sort sprites to have higher x first (which will be drawn first)
    std::sort(spritesToDraw.begin(), spritesToDraw.end(), [](SpriteAttribute &a, SpriteAttribute &b)
    {
        return a.x > b.x;
    });

    const uint16 tilesetAddr = 0x8000;
    // Sprite are ready to be drawn first to last
    for (auto sprite : spritesToDraw)
    {
        Vector2i screenPosition { sprite.x - 8, sprite.y - 16};
        const uint8 currentLine = memory.LY;
        const size_t lineInSprite = currentLine - screenPosition.y;

        const bool XFlip = sprite.flag & spriteAttributeFlagBits.XFlip;
        const bool YFlip = sprite.flag & spriteAttributeFlagBits.YFlip;
        const uint16 paletteAddr = (sprite.flag & spriteAttributeFlagBits.paletteNumber) ? 0xFF49 : 0xFF48;
        const uint8 palette = memory.read8(paletteAddr);

        // On 8x16 sprite mode, LSB is ignored
        uint8 tilesetId = sprite.tilesetId;
        if (areSpritesBig)
        {
            tilesetId &= ~(1u);
        }
        const uint16 currentTileAddr = (tilesetId * bytesPerSprite) + tilesetAddr;
        const uint16 currentTileLineAddr = currentTileAddr + lineInSprite * 2;
        for (size_t i = 0; i < 8; ++i)
        {
            Vector2i pixelScreenPosition { screenPosition.x + static_cast<int32>(i), memory.LY};

            if (pixelScreenPosition.x < 0 || pixelScreenPosition.x > SCREEN_WIDTH
                || pixelScreenPosition.y < 0 || pixelScreenPosition.y > SCREEN_HEIGHT)
            {
                continue;
            }

            // Get position inside the tile
            const uint8 tilePosX = XFlip ? i : 7 - i; // says which bit to read
            const uint8 tilePosY = YFlip ? (spriteSize.y * 2) - lineInSprite * 2 : lineInSprite * 2; // says which bytes (two bytes per line) to read

            // address of both bytes for the current line of the tile
            const uint8 tile0 = memory.read8(currentTileAddr + tilePosY + 0);
            const uint8 tile1 = memory.read8(currentTileAddr + tilePosY + 1);

            // actual DMG color of the pixel [0;3]
            const uint8 paletteIndex = ((tile0 >> tilePosX) & 1u) + (((tile1 >> tilePosX) & 1u) << 1u);
            const uint8 paletteColor = (palette >> (paletteIndex * 2u) & 1u) + ((palette >> (paletteIndex * 2u + 1) & 1u) << 1u);

            // We do not have priority, draw only if background & window below pixel is 0
            if (sprite.flag & spriteAttributeFlagBits.priority)
            {
                if (colors[memory.backgroundPalette & 3u][0] != buffer[(pixelScreenPosition.x + (pixelScreenPosition.y * SCREEN_WIDTH)) * 3 + 0])
                {
                    continue;
                }
            }

            // Get the SFML color
            std::array<uint8, 3> SFMLColor = colors[paletteColor];
            if (paletteColor == 0)
            {
                continue;
            }
            // print it in the final buffer
            buffer[(pixelScreenPosition.x + (pixelScreenPosition.y * SCREEN_WIDTH)) * 3 + 0] = SFMLColor[0];
            buffer[(pixelScreenPosition.x + (pixelScreenPosition.y * SCREEN_WIDTH)) * 3 + 1] = SFMLColor[1];
            buffer[(pixelScreenPosition.x + (pixelScreenPosition.y * SCREEN_WIDTH)) * 3 + 2] = SFMLColor[2];
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
