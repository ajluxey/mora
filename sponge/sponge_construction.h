//
// Created by Alexey on 05.11.2022.
//

#ifndef MORA_SPONGE_CONSTRUCTION_H
#define MORA_SPONGE_CONSTRUCTION_H

#include "../mora/byte_utils.h"
#include "../mora/mora.h"
#include <cmath>


#define STATE_BYTE_SIZE 8
#define STATE_BIT_SIZE 64
#define R_BIT_SIZE 32
#define INTEGER_QUANTITY_BYTES_FOR_R_BLOCK (int) ceil(R_BIT_SIZE/8)

typedef struct sponge_struct {
    uint64_t state;
    uint8_t r_bit_size = R_BIT_SIZE;
    uint8_t state_bit_size = STATE_BIT_SIZE;
} sponge_struct;

void absorbing(sponge_struct *sponge, uint64_t *block);
uint64_t squeezing(sponge_struct *sponge);

#endif //MORA_SPONGE_CONSTRUCTION_H
