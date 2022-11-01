#include "byte_utils.h"

uint16_t half_bytes_to_16bit(const uint8_t *original) {
    uint16_t result = 0;
    for (int i = 0; i < 4; i++) {
        result = (result << 4) + original[i];
    }
    return result;
}


uint8_t get_ith_bit(uint16_t x, size_t i)
{
    return (x & (1 << (16 - i - 1))) >> (16 - i - 1);
}


void split_into_half_bytes(const uint8_t *bytes, uint8_t *result) {
    int i;
    for (i = 0; i < BYTES_IN_BLOCK; i++) {
        result[i * 2] = bytes[i] >> 4;
        result[i * 2 + 1] = bytes[i] & 0xf;
    }
}


void join_half_bytes(const uint8_t *half_bytes, uint8_t *result) {
    int i;
    for (i = 0; i < HALF_BYTES_IN_BLOCK; i++) {
        result[i] = half_bytes[i * 2] * 0xf + half_bytes[i * 2 + 1];
    }
}


void adding_in_Z64(const uint8_t *first, const uint8_t *second, uint8_t *result) {
    uint8_t overflowed_4bit = 0;
    for (int i = HALF_BYTES_IN_BLOCK - 1; i >= 0; i--) {
        overflowed_4bit = first[i] + second[i] + (overflowed_4bit >> 4);
        result[i] = overflowed_4bit & 0xf;
    }
}


void X(const uint8_t *first, const uint8_t *second, uint8_t *result) {
    int i;
    for(i = 0; i < HALF_BYTES_IN_BLOCK; i++) {
        result[i] = first[i] ^ second[i];
    }
}


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


void LPS(uint8_t *original) {
    S(original);
    P(original);
    L(original);
}


void LPSX(uint8_t *first, uint8_t *second, uint8_t *result) {
    X(first, second, result);
    LPS(result);
}


void bit_length_of_short_message_in_half_bytes_vector(size_t bytes_count, uint8_t *result) {
    size_t bits_count = bytes_count * 8;
    for (int i = HALF_BYTES_IN_BLOCK - 1; i > -1; i--) {
        result[i] = bits_count & 0xf;
        bits_count = bits_count >> 4;
    }
}
