#ifndef FRACTAL_I_INPUT_H
#define FRACTAL_I_INPUT_H

#include "../../general.h"

/**
 * IInput is an interface which allow to retrieve buttons status.
 */
class IInput
{
public:
    /**
     * Set these values to `true` when one of these buttons get pressed
     */
    struct InputStatus
    {
        bool down = false;
        bool up = false;
        bool left = false;
        bool right = false;
        bool start = false;
        bool select = false;
        bool buttonA = false;
        bool buttonB = false;
    };

    InputStatus inputStatus;
};

#endif //FRACTAL_I_INPUT_H
