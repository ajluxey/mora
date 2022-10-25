#include "LPS.h"

void S(uint8_t *original) {       // Based on Pi, S-box
    for (int i = 0; i < HALF_BYTES_IN_BLOCK; i++) {
        original[i] = Pi[original[i]];
    }
}


void P(uint8_t *original) {  // Based on Tau (half bytes shake)
    int i;
    half_bytes_vector result;
    for (i = 0; i < HALF_BYTES_IN_BLOCK; i++) {
        result[i] = original[Tau[i]];
    }
    memcpy(original, result, HALF_BYTES_IN_BLOCK);
}


uint16_t l_part(uint16_t original) {
    uint16_t result = 0;
    for (size_t i = 0; i < 16; i++) {
        if (get_ith_bit(original, i) == 1) {
            result ^= A[i];
        }
    }
    return result;
}


void L(uint8_t *original) {
    int i, j;
    for (i = 0; i < 4; i++) {                               // for each iteration, we concat 4 half bytes to one uint16_t
        uint16_t bits16;

        uint8_t *shifted_original = original + i * 4;       // shift = 4 half bytes
        bits16 = half_bytes_to_16bit(shifted_original);     //

        uint16_t part_result;
        part_result = l_part(bits16);

        for (j = 0; j < 4; j++) {
            original[4 * i + j] = (part_result & (0xF << (4 * (4 - j -1)))) >> (4 * (4 - j - 1));
        }
    }
}
