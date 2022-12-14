#ifndef MORA_CONSTANTS_H
#define MORA_CONSTANTS_H

#include <cstdint>

#define BYTES_IN_BLOCK 8
#define HALF_BYTES_IN_BLOCK 16

typedef uint8_t bytes_vector[BYTES_IN_BLOCK];
typedef uint8_t half_bytes_vector[HALF_BYTES_IN_BLOCK];

static const uint8_t Pi[HALF_BYTES_IN_BLOCK] = {
        15, 9,  1,  7,
        13, 12, 2,  8,
        6,  5,  14, 3,
        0,  11, 4,  10
};

static const uint8_t Tau[HALF_BYTES_IN_BLOCK] = {
        0, 4, 8,  12,
        1, 5, 9,  13,
        2, 6, 10, 14,
        3, 7, 11, 15
};

static const uint16_t A[HALF_BYTES_IN_BLOCK] = {
        0x3a22, 0x8511, 0x4b99, 0x2cdd,
        0x483b, 0x248c, 0x1246, 0x9123,
        0x59e5, 0xbd7b, 0xcfac, 0x6e56,
        0xac52, 0x56b1, 0xb3c9, 0xc86d
};

static const uint8_t C[9*HALF_BYTES_IN_BLOCK] = {
        0xc, 0x0, 0x1, 0x6, 0x4, 0x6, 0x3, 0x3, 0x5, 0x7, 0x5, 0xa, 0x9, 0x6, 0x9, 0x9,
        0x9, 0x2, 0x5, 0xb, 0x4, 0xe, 0xf, 0x4, 0x9, 0xa, 0x5, 0xe, 0x7, 0x1, 0x7, 0x4,
        0x8, 0x6, 0xa, 0x8, 0x9, 0xc, 0xd, 0xc, 0xf, 0x6, 0x7, 0x3, 0xb, 0xe, 0x2, 0x6,
        0x1, 0x8, 0x8, 0x5, 0x5, 0x5, 0x8, 0xf, 0x0, 0xe, 0xa, 0xc, 0xa, 0x3, 0xf, 0x1,
        0xd, 0xc, 0xf, 0xc, 0x5, 0xb, 0x8, 0x9, 0xe, 0x3, 0x5, 0xe, 0x8, 0x4, 0x3, 0x9,
        0x5, 0x4, 0xb, 0x9, 0xe, 0xd, 0xc, 0x7, 0x8, 0x9, 0x4, 0x6, 0x4, 0xd, 0x2, 0x3,
        0xf, 0x8, 0x0, 0xd, 0x4, 0x9, 0xa, 0xf, 0xd, 0xe, 0x0, 0x4, 0x4, 0xb, 0xf, 0x9,
        0x8, 0xc, 0xb, 0xb, 0xd, 0xf, 0x7, 0x1, 0xc, 0xc, 0xa, 0xa, 0x4, 0x3, 0xf, 0x1,
        0xc, 0xb, 0x4, 0x3, 0xa, 0xf, 0x7, 0x2, 0x2, 0xc, 0xb, 0x5, 0x2, 0x0, 0xb, 0x9
};

static const half_bytes_vector ADDED_TO_N = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x4, 0x0};

#endif //MORA_CONSTANTS_H
