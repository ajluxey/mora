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


// При выполнении E изменяется K и hash
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


// При выполнении G из изначальных переменных изменяется лишь hash
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


typedef struct mora_context {
    half_bytes_vector N;
    half_bytes_vector sigma;
    half_bytes_vector current_block;
    half_bytes_vector current_hash;
} mora_context;


mora_context init_context() {
    mora_context context;
    half_bytes_vector IV = {0};
    half_bytes_vector N = {0};
    half_bytes_vector sigma = {0};

    memcpy(context.current_hash, IV, HALF_BYTES_IN_BLOCK);
    memcpy(context.N, N, HALF_BYTES_IN_BLOCK);
    memcpy(context.sigma, sigma, HALF_BYTES_IN_BLOCK);

    return context;
}


void padding_short_block(uint8_t *short_message, size_t length, uint8_t *result) {
    memset(result, 0x00, BYTES_IN_BLOCK);
    memcpy(result + BYTES_IN_BLOCK - length, short_message, length);
    result[BYTES_IN_BLOCK - length - 1] = 0x1;
}


void mora(uint8_t *IV, uint8_t *original_message, size_t current_length) {
    // first part
    mora_context context = init_context();
    split_into_half_bytes(IV, context.current_hash);

    // second part
    while (current_length >= BYTES_IN_BLOCK) {
        size_t shift;
        shift = current_length - BYTES_IN_BLOCK;

        split_into_half_bytes(original_message + shift, context.current_block); // сплитим и записываем байты! с original_message[shift] до original_message[shift + 8]

        G(context.current_hash, context.current_block, context.N);
        adding_in_Z64(context.N, ADDED_TO_N, context.N);
        adding_in_Z64(context.sigma, context.current_block, context.sigma);

        current_length -= shift;
    }
    // last part
    if (current_length == 0) {
        join_half_bytes(context.current_hash, IV);
        return;
    }
    half_bytes_vector last_added_to_N;
    bit_length_of_short_message_in_half_bytes_vector((uint8_t) current_length, last_added_to_N);
    padding_short_block(original_message, current_length, context.current_block);

    G(context.current_hash, context.current_block, context.N);
    adding_in_Z64(context.N, last_added_to_N, context.N);
    adding_in_Z64(context.sigma, context.current_block, context.sigma);

    memcpy(context.current_block, context.current_hash, HALF_BYTES_IN_BLOCK);
    memset(context.N, 0x0, HALF_BYTES_IN_BLOCK);
    G(context.current_hash, context.current_block, context.N);

    G(context.current_hash, context.sigma, context.N);

    join_half_bytes(context.current_hash, IV);
}
