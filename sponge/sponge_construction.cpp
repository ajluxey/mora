//
// Created by Alexey on 05.11.2022.
//

#include <cstdio>
#include "sponge_construction.h"


#define HALF_BYTE_SHIFT 4


void state_to_half_bytes(const uint64_t *state, uint8_t *half_bytes_state) {
    size_t half_bytes_in_state = STATE_BYTE_SIZE * 2;

    for (int i = 0; i < half_bytes_in_state; i++) {
        half_bytes_state[i] = (*state & (0xFULL << (HALF_BYTE_SHIFT * (half_bytes_in_state - i - 1)))) >> (HALF_BYTE_SHIFT * (half_bytes_in_state - i - 1));
    }
}


void state_from_half_bytes(const uint8_t *half_bytes_state, uint64_t *state) {
    *state = 0;
    size_t half_bytes_in_state = STATE_BYTE_SIZE * 2;

    for (int i = 0; i < half_bytes_in_state; i++) {
        *state = (*state << HALF_BYTE_SHIFT) + half_bytes_state[i];
    }
}


void sponge_function(uint8_t *state) {
    half_bytes_vector mora_constant;
    for (int i = 0; i < 9; i++) {
        Ci(mora_constant, i);
        LPSX(state, mora_constant, state);
    }
}


// E.g. |state| - 8 bit, |Sr| - 3 bit
// init input = 0b00000101 -> 0b10100000 for correct xoring with Sr block in state
void sponge_X(uint64_t *state, const uint64_t *input) {
    *state = *state ^ (*input << (STATE_BIT_SIZE - R_BIT_SIZE));
}


void absorbing(sponge_struct *sponge, uint64_t *block) {
    sponge_X(&sponge->state, block);

    half_bytes_vector state;
    state_to_half_bytes(&sponge->state, state);

    sponge_function(state);

    state_from_half_bytes(state, &sponge->state);
}


uint64_t squeezing(sponge_struct *sponge) {
    half_bytes_vector state;
    state_to_half_bytes(&sponge->state, state);

    sponge_function(state);

    state_from_half_bytes(state, &sponge->state);
    return sponge->state >> (STATE_BIT_SIZE - R_BIT_SIZE);
}
