#include "sponge_crypto.h"


sponge_struct init_sponge() {
    sponge_struct sponge;
    sponge.state = 0;
    return sponge;
}


uint64_t join_n_bytes(const uint8_t *bytes, size_t n) {
    uint64_t result = 0;
    for (size_t i = 0; i < n; i++) {
        result = (result << 8) + bytes[i];
    }
    return result;
}


void fill_buffer(uint8_t *remaining_message, size_t message_remaining_byte_length,
                 size_t count_of_needed_bytes, uint64_t *buffer, size_t buffer_remaining_bit_length) {
    uint64_t new_message_part;

    if (message_remaining_byte_length >= count_of_needed_bytes) {
        new_message_part = join_n_bytes(remaining_message, count_of_needed_bytes);
    } else {
        uint8_t padding_message[8] = {0};
        memcpy(padding_message, remaining_message, message_remaining_byte_length);
        padding_message[message_remaining_byte_length] = 0x8; // padding with 0b1000...

        new_message_part = join_n_bytes(padding_message, count_of_needed_bytes);
    }
    *buffer = (*buffer << buffer_remaining_bit_length) + new_message_part;
}


void sponge_hash(uint8_t *message, size_t message_remaining_byte_length, uint8_t *result) {
    sponge_struct sponge = init_sponge();
    uint64_t buffer = 0;
    size_t buffer_remaining_bit_length = 0;
    size_t bytes_processed = 0;

    uint64_t r_block = 0;
    size_t count_of_needed_bytes;
    while (true) {
        if (message_remaining_byte_length <= 0 && buffer_remaining_bit_length == 0) {
            break;
        }

        if (buffer_remaining_bit_length < R_BIT_SIZE) {
            count_of_needed_bytes = (size_t) ceil((float)(R_BIT_SIZE - buffer_remaining_bit_length) / 8);
            fill_buffer(message + bytes_processed, message_remaining_byte_length, count_of_needed_bytes, &buffer, buffer_remaining_bit_length);
            message_remaining_byte_length -= count_of_needed_bytes;
            bytes_processed += count_of_needed_bytes;
            buffer_remaining_bit_length += count_of_needed_bytes * 8;
        }
        r_block = buffer >> (buffer_remaining_bit_length - R_BIT_SIZE);
        absorbing(&sponge, &r_block);
        buffer_remaining_bit_length -= R_BIT_SIZE;
    }

    for(int i = 0; i < 8; i++) {
        result[i] = (sponge.state & (0xFF << (8 - i - 1))) >> (8 - i - 1);
    }
}










/*
void split_into_half_bytes_sponge(const uint8_t *bytes, uint8_t *result) {
    for (int i = 0; i < BYTES_R_SIZE; i++) {
        result[i * 2] = bytes[i] >> 4;
        result[i * 2 + 1] = bytes[i] & 0xf;
    }
}


typedef struct sponge_context {
    half_bytes_sponge sponge;
    uint8_t current_block[R_SIZE];
    half_bytes_vector result;
} sponge_context;


sponge_context init_sponge_context() {
    sponge_context context;
    memset(context.sponge.state, 0x0, STATE_SIZE);
    return context;
}


void new_padding_block(uint8_t *message, size_t length, uint8_t *destination) {
    memset(destination, 0x0, R_SIZE);
    memcpy(destination, message, length);
}


int new_block(uint8_t *message, size_t max_length, size_t current_pos, uint8_t *destination) {
    if (current_pos >= max_length) {
        return  0;
    }

    size_t diff = max_length - current_pos;
    if (diff >= BYTES_R_SIZE) {
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
        current_length += BYTES_R_SIZE;
    }
    for (int i = 0; i < STATE_SIZE / R_SIZE; i++) {
        squeezing(&context.sponge, context.result + i * R_SIZE);
    }
    join_half_bytes(context.result, result);
}


void sponge_MAC(uint8_t *message, size_t length, uint8_t *key, size_t key_length, uint8_t *result) {
    sponge_context context = init_sponge_context();

    size_t current_length = 0;
    while (new_block(key, key_length, current_length, context.current_block)){
        absorbing(&context.sponge, context.current_block);
        current_length += BYTES_R_SIZE;
    }

    current_length = 0;
    while (new_block(message, length, current_length, context.current_block)) {
        absorbing(&context.sponge, context.current_block);
        current_length += BYTES_R_SIZE;
    }
    for (int i = 0; i < STATE_SIZE / R_SIZE; i++) {
        squeezing(&context.sponge, context.result + i * R_SIZE);
    }
    join_half_bytes(context.result, result);
}
*/
