#include "sponge_crypto.h"


void split_into_half_bytes_sponge(const uint8_t *bytes, uint8_t *result) {
    for (int i = 0; i < R_SIZE; i++) {
        result[i * 2] = bytes[i] >> 4;
        result[i * 2 + 1] = bytes[i] & 0xf;
    }
}


typedef struct sponge_context {
    half_bytes_sponge sponge;
    uint8_t current_block[R_SIZE*2];
    half_bytes_vector result;
} sponge_context;


sponge_context init_sponge_context() {
    sponge_context context;
    memset(context.sponge.state, 0x0, STATE_SIZE * 2);
    return context;
}


void new_padding_block(uint8_t *message, size_t length, uint8_t *destination) {
    memset(destination, 0x0, R_SIZE * 2);
    memcpy(destination, message, length);
}


int new_block(uint8_t *message, size_t max_length, size_t current_pos, uint8_t *destination) {
    int diff = max_length - current_pos;
    if (diff <= 0) {
        return 0;
    }
    if (diff >= R_SIZE) {
        split_into_half_bytes_sponge(message + current_pos, destination);
    } else {
        new_padding_block(message + current_pos, diff, destination);
    }
    return 1;
}


void sponge_hash(uint8_t *message, size_t length, uint8_t *result) {
    sponge_context context = init_sponge_context();
    size_t current_length = 0;
    while (new_block(message, length, current_length, context.current_block)) {
        absorbing(&context.sponge, context.current_block);
        current_length += R_SIZE;
    }
    for (int i = 0; i < STATE_SIZE / R_SIZE; i++) {
        squeezing(&context.sponge, context.result + i * R_SIZE * 2);
    }
    join_half_bytes(context.result, result);
}


void sponge_MAC(uint8_t *message, size_t length, uint8_t *key, size_t key_length, uint8_t *result) {
    sponge_context context = init_sponge_context();

    size_t current_length = 0;
    while (new_block(key, key_length, current_length, context.current_block)){
        absorbing(&context.sponge, context.current_block);
        current_length += R_SIZE;
    }

    current_length = 0;
    while (new_block(message, length, current_length, context.current_block)) {
        absorbing(&context.sponge, context.current_block);
        current_length += R_SIZE;
    }
    for (int i = 0; i < STATE_SIZE / R_SIZE; i++) {
        squeezing(&context.sponge, context.result + i * R_SIZE * 2);
    }
    join_half_bytes(context.result, result);
}