#ifndef MORA_BYTE_UTILS_H
#define MORA_BYTE_UTILS_H

#include "constants.h"
#include <cstring>

uint16_t half_bytes_to_16bit(const uint8_t *original);
uint8_t get_ith_bit(uint16_t x, size_t i);
void split_into_half_bytes(const uint8_t *bytes, uint8_t *result);
void join_half_bytes(const uint8_t *half_bytes, uint8_t *result);
void adding_in_Z64(const uint8_t *first, const uint8_t *second, uint8_t *result);
void X(const uint8_t *first, const uint8_t *second, uint8_t *result);
void S(uint8_t *original);
void P(uint8_t *original);
void L(uint8_t *original);
uint16_t l_part(uint16_t original);
void LPS(uint8_t *original);
void LPSX(uint8_t *first, uint8_t *second, uint8_t *result);

#endif //MORA_BYTE_UTILS_H
