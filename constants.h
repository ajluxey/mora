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

static const unsigned long long C[BYTES_IN_BLOCK] = {
        0xc0164633575a9699,
        0x925b4ef49a5e7174,
        0x86a89cdcf673be26,
        0x1885558f0eaca3f1,
        0xdcfc5b89e35e8439,
        0x54b9edc789464d23,
        0xf80d49afde044bf9,
        0x8cbbdf71ccaa43f1
};

#endif //MORA_CONSTANTS_H
