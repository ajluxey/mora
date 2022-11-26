#include <cstdio>
#include <iostream>
#include "mora.h"

#define CONSTANTS_COUNT 9

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
    for(size_t i = 0; i < CONSTANTS_COUNT; i++) {
        LPS(hash);
        next_K(K, i);
        X(hash, K, hash);
    }
}


// При выполнении G из изначальных переменных изменяется лишь hash
void G(uint8_t *hash, uint8_t *block, uint8_t *N) {
    half_bytes_vector K = {0};
    half_bytes_vector start_hash;
    memcpy(start_hash, hash, HALF_BYTES_IN_BLOCK);

    LPSX(hash, N, K);
    E(K, block, hash);

    X(hash, start_hash, hash);
    X(hash, block, hash);
}


typedef struct mora_context {
    half_bytes_vector N;                // количество обработанных бит
    half_bytes_vector sigma;
    half_bytes_vector current_block;    // текущий блок на каждой итерации G
    half_bytes_vector current_hash;     // хэш - изменяемый в G с помощью ключей и передаваемый далее
} mora_context;


mora_context init_context() {
    mora_context context;

    memset(context.current_hash, 0x0, HALF_BYTES_IN_BLOCK);
    memset(context.N, 0x0, HALF_BYTES_IN_BLOCK);
    memset(context.sigma, 0x0, HALF_BYTES_IN_BLOCK);

    return context;
}


void padding_short_block(uint8_t *short_message, size_t length, uint8_t *result) {
    memset(result, 0x00, BYTES_IN_BLOCK);
    memcpy(result + BYTES_IN_BLOCK - length, short_message, length);
    result[BYTES_IN_BLOCK - length - 1] = 0x1;
}


void mora(uint8_t *IV, uint8_t *original_message, size_t current_length, uint8_t *result) {
    // first part
    mora_context context = init_context();
    split_into_half_bytes(IV, context.current_hash);

    // second part
    while (current_length >= BYTES_IN_BLOCK) {
        size_t shift;
        shift = current_length - BYTES_IN_BLOCK;

        split_into_half_bytes(original_message + shift, context.current_block); // сплитим !байты! и записываем с original_message[shift] до original_message[shift + 8] в массив полубайт

        G(context.current_hash, context.current_block, context.N);
        adding_in_Z64(context.N, ADDED_TO_N, context.N);
        adding_in_Z64(context.sigma, context.current_block, context.sigma);

        current_length -= BYTES_IN_BLOCK;
    }
    // last part
    half_bytes_vector last_added_to_N;      // битовая длина оставшегося блока
    bit_length_of_short_message_in_half_bytes_vector(current_length, last_added_to_N);
    padding_short_block(original_message, current_length, context.current_block);

    G(context.current_hash, context.current_block, context.N);
    adding_in_Z64(context.N, last_added_to_N, context.N);           // N = длина сообщения в битах
    adding_in_Z64(context.sigma, context.current_block, context.sigma);

    // mixing length of original message
    memcpy(context.current_block, context.N, HALF_BYTES_IN_BLOCK);
    memset(context.N, 0x0, HALF_BYTES_IN_BLOCK);                // N теперь содержит нулевой вектор
    G(context.current_hash, context.current_block, context.N); // context.N = {0}; context.current_block = length of message in bit (in half bytes vector)

    G(context.current_hash, context.sigma, context.N);

    join_half_bytes(context.current_hash, result);
}
