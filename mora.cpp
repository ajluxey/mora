#include <cstdio>
#include <iostream>
#include "mora.h"


void print_vector(uint8_t *vector) {
    int i;
    for (i = 0; i < 16; i++)
        printf("%x", vector[i]);
    std::cout << "\n";
}


void Ci(uint8_t *constant, size_t i) {
    memcpy(constant, C + i * HALF_BYTES_IN_BLOCK, HALF_BYTES_IN_BLOCK);
}


void next_K(uint8_t *current_K, size_t current_iteration) {
    half_bytes_vector constant;
    Ci(constant, current_iteration);

    LPSX(current_K, constant, current_K);
}


void E(uint8_t *K, uint8_t* block, uint8_t *hash) {
    X(K, block, hash);
    for(size_t i = 0; i < 9; i++) {
        LPS(hash);
        next_K(K, i);
        printf("K%d = ", i + 2);
        print_vector(K);
        X(hash, K, hash);
    }
}



void G(uint8_t *hash, uint8_t *block, uint8_t *N) {
    half_bytes_vector K = {0};
    half_bytes_vector start_hash;
    memcpy(start_hash, hash, HALF_BYTES_IN_BLOCK);

    LPSX(hash, N, K);
    printf("K1 = ");
    print_vector(K);
    E(K, block, hash);

    X(hash, start_hash, hash);
    X(hash, block, hash);
}


// original message - bytes array; NOT HALF BYTES
void mora(uint8_t *IV, uint8_t *original_message, size_t current_length) {
    half_bytes_vector N = {0};
    half_bytes_vector hash;

    split_into_half_bytes(IV, hash);

    bytes_vector current_block;
    half_bytes_vector current_block16;
    while (current_length > BYTES_IN_BLOCK) {
        size_t shift;
        shift = current_length - BYTES_IN_BLOCK;

        memcpy(current_block, original_message + shift, BYTES_IN_BLOCK);
        split_into_half_bytes(current_block, current_block16);
        G(hash, current_block16, N);

        current_length = shift;
    }
    // можно ли как-то скопировать, а остальное заполнить нулями?
    for (int i = 0; i < BYTES_IN_BLOCK; i++) {
        current_block[i] = 0;
    }
    memcpy(current_block, original_message, current_length);
    //
    split_into_half_bytes(current_block, current_block16);
    G(hash, current_block16, N);
}
