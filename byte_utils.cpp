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


void bytes_xor(const uint8_t *first, const uint8_t *second, uint8_t *result) {
    int i;
    for(i = 0; i < HALF_BYTES_IN_BLOCK; i++) {
        result[i] = first[i] ^ second[i];
    }
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