//
// Created by Alexey on 05.11.2022.
//

#ifndef MORA_SPONGE_CONSTRUCTION_H
#define MORA_SPONGE_CONSTRUCTION_H

#include "../mora/byte_utils.h"
#include "../mora/mora.h"

#define STATE_SIZE 8
#define R_SIZE 4
#define C_SIZE STATE_SIZE - R_SIZE


typedef struct half_bytes_sponge {
    half_bytes_vector state;
    uint8_t *r_block = state;
    uint8_t *c_block = state + R_SIZE * 2;
} half_bytes_sponge;


void absorbing(half_bytes_sponge *sponge, uint8_t *block);
void squeezing(half_bytes_sponge *sponge, uint8_t *result_block);

#endif //MORA_SPONGE_CONSTRUCTION_H
