//
// Created by Alexey on 05.11.2022.
//

#include <cstdio>
#include "sponge_construction.h"


#define HALF_BYTE_SHIFT 4


void state_to_half_bytes(const uint64_t *state, uint8_t *half_bytes_state) {
    size_t half_bytes_in_state = STATE_BYTE_SIZE * 2;

    for (int i = 0; i < half_bytes_in_state; i++) {
        half_bytes_state[i] = (*state & (0xF << (HALF_BYTE_SHIFT * (half_bytes_in_state - i - 1)))) >> (HALF_BYTE_SHIFT * (half_bytes_in_state - i - 1));
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
    printf("XOR:\n");
    printf("xorable = %016lx\n", &input);
    *state = *state ^ (*input << (STATE_BIT_SIZE - R_BIT_SIZE));
    printf("state = %016lx\n", *state);
}


void absorbing(sponge_struct *sponge, uint64_t *block) {
    printf("\nstate = %016lx\n", sponge->state);
    printf("ABSORBING:\n");

    sponge_X(&sponge->state, block);

    half_bytes_vector state;
    state_to_half_bytes(&sponge->state, state);

    printf("SPONGE FUNC:\n");
    sponge_function(state);

    state_from_half_bytes(state, &sponge->state);
    printf("state = %016lx\n", sponge->state);
}


uint64_t squeezing(sponge_struct *sponge) {
    half_bytes_vector state;
    state_to_half_bytes(&sponge->state, state);

    sponge_function(state);

    state_from_half_bytes(state, &sponge->state);
    return sponge->state >> (STATE_BIT_SIZE - R_BIT_SIZE);
}


/*
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
*/
