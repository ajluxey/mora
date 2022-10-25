#ifndef MORA_BYTE_UTILS_H
#define MORA_BYTE_UTILS_H

#include "constants.h"

uint16_t half_bytes_to_16bit(const uint8_t *original);
uint8_t get_ith_bit(uint16_t x, size_t i);
void bytes_xor(const uint8_t *first, const uint8_t *second, uint8_t *result);
void split_into_half_bytes(const uint8_t *bytes, uint8_t *result);
void join_half_bytes(const uint8_t *half_bytes, uint8_t *result);

#endif //MORA_BYTE_UTILS_H
