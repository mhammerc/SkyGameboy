#ifndef FRACTAL_INPUT_MANAGER_H
#define FRACTAL_INPUT_MANAGER_H

#include "../general.h"
#include "../frontend/interfaces/i_input.h"
#include "virtual_memory.h"

#include "dbg.h"

class InputManager
{
public:
    explicit InputManager(VirtualMemory &memory, IInput &input) : memory(memory), input(input)
    {};

    void updateInputStatus()
    {
        IInput::InputStatus filteredRawInput = input.inputStatus;
        const uint8 previousJoypadButtons = memory.joypadButtons;

        // Direction buttons are disabled
        if (memory.joypadButtons & memory.joypadButtonsBits.selectDirection)
        {
            filteredRawInput.down = false;
            filteredRawInput.up = false;
            filteredRawInput.left = false;
            filteredRawInput.right = false;
        }
        // Buttons are disabled
        if (memory.joypadButtons & memory.joypadButtonsBits.selectButton)
        {
            filteredRawInput.start = false;
            filteredRawInput.select = false;
            filteredRawInput.buttonA = false;
            filteredRawInput.buttonB = false;
        }

        if (filteredRawInput.up || filteredRawInput.select)
        {
            memory.joypadButtons &= ~memory.joypadButtonsBits.upOrSelect;
        }
        else
        {
            memory.joypadButtons |= memory.joypadButtonsBits.upOrSelect;
        }

        if (filteredRawInput.down || filteredRawInput.start)
        {
            memory.joypadButtons &= ~memory.joypadButtonsBits.downOrStart;
        }
        else
        {
            memory.joypadButtons |= memory.joypadButtonsBits.downOrStart;
        }

        if (filteredRawInput.right || filteredRawInput.buttonA)
        {
            memory.joypadButtons &= ~memory.joypadButtonsBits.rightOrA;
        }
        else
        {
            memory.joypadButtons |= memory.joypadButtonsBits.rightOrA;
        }

        if (filteredRawInput.left || filteredRawInput.buttonB)
        {
            memory.joypadButtons &= ~memory.joypadButtonsBits.leftOrB;
        }
        else
        {
            memory.joypadButtons |= memory.joypadButtonsBits.leftOrB;
        }

        // Request for interrupt on falling edge
        if ((previousJoypadButtons & memory.joypadButtonsBits.inputBits) == memory.joypadButtonsBits.inputBits
            && (memory.joypadButtons & memory.joypadButtonsBits.inputBits) != memory.joypadButtonsBits.inputBits)
        {
            dbg("trigger joypad");
            memory.interruptRequest |= memory.interruptBits.joypad;
        }
    };

private:
    VirtualMemory &memory;
    IInput &input;

};

#endif //FRACTAL_INPUT_MANAGER_H
