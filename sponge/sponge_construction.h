//
// Created by Alexey on 05.11.2022.
//

#ifndef MORA_SPONGE_CONSTRUCTION_H
#define MORA_SPONGE_CONSTRUCTION_H

#include "../mora/byte_utils.h"
#include "../mora/mora.h"

#define BYTES_STATE_SIZE 8
#define BYTES_R_SIZE 1
#define STATE_SIZE BYTES_STATE_SIZE * 2
#define R_SIZE BYTES_R_SIZE * 2

#define C_SIZE STATE_SIZE - R_SIZE


typedef struct half_bytes_sponge {
    half_bytes_vector state;
    uint8_t *r_block = state;
    uint8_t *c_block = state + R_SIZE;
} half_bytes_sponge;


void absorbing(half_bytes_sponge *sponge, uint8_t *block);
void squeezing(half_bytes_sponge *sponge, uint8_t *result_block);

#endif //MORA_SPONGE_CONSTRUCTION_H
