//
// Created by Alexey on 05.11.2022.
//

#include <cstdio>
#include "sponge_construction.h"


void print_sponge_state(uint8_t *state){
    printf("sponge state:\n");
    for (int i = 0; i < STATE_SIZE; i++){
        printf("%x ", state[i]);
        if (i == 7) printf(" ");
    }
    printf("\n");
}



void sponge_X(const uint8_t *first, const uint8_t *second, uint8_t *result) {
    for (int i = 0; i < R_SIZE; i++) {
        result[i] = first[i] ^ second[i];
    }
}


void sponge_function(uint8_t *state) {
    half_bytes_vector mora_constant;
    for (int i = 0; i < STATE_SIZE; i++) {
        Ci(mora_constant, i);
        LPSX(state, mora_constant, state);
    }
}


void absorbing(half_bytes_sponge *sponge, uint8_t *block) {
    sponge_X(sponge->r_block, block, sponge->r_block);
    sponge_function(sponge->state);
}


void squeezing(half_bytes_sponge *sponge, uint8_t *result_block) {
    sponge_function(sponge->state);
    memcpy(result_block, sponge->r_block, R_SIZE);
}
