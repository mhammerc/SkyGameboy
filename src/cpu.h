#ifndef FRACTAL_CPU_H
#define FRACTAL_CPU_H

#include "general.h"

class CPU
{
private:
    // registers
    uint8 A, F;
    uint8 B, C;
    uint8 D, E;
    uint8 H, L;
    uint16 SP;
    uint16 PC;
};


#endif //FRACTAL_CPU_H
