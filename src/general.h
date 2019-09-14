#ifndef FRACTAL_GENERAL_H
#define FRACTAL_GENERAL_H

typedef unsigned char uint8;
typedef char int8;

typedef unsigned short uint16;
typedef short int16;

/**
 * Parse two independant bytes to a word (16bit value) using little endian.
 *
 * @param b1 byte 1
 * @param b2 byte 2
 * @return 16 bit value as 0x[b2][b1]
 */
[[maybe_unused]] static uint16 bytesToWordLE(const uint8 b1, const uint8 b2)
{
    return static_cast<uint16>(b1) + (static_cast<uint16>(b2) << 8u);
}

#endif //FRACTAL_GENERAL_H
