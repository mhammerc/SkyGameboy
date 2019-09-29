#ifndef FRACTAL_I_DISPLAY_H
#define FRACTAL_I_DISPLAY_H

#include <vector>

#include "../../general.h"

/**
 * IDisplay is an interface which allow for frames to be drawn on screen.
 */
class IDisplay
{
public:
    /**
    * This function get called when a new frame have been generated.
    *
    * Vector length will always be length `69120`: 160 * 144 * 3 (RGB).
    * The given vector will always be the same. If you need to long-live the frame,
    * you may need to copy it. According to your display implementation you may also need to copy
    * the vector and add the alpha component (RGBA).
    *
    * @param frame Frame' pixels as described above.
    */
    virtual void newFrameIsReady(const std::vector<uint8> &frame) = 0;
};

#endif //FRACTAL_I_DISPLAY_H
