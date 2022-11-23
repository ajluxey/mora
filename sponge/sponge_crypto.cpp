#include "sponge_crypto.h"


typedef struct sponge_context {
    uint8_t *message;
    size_t message_length;
    size_t bytes_processed;

    uint64_t buffer;
    size_t buffer_remaining_bit_length;

    uint64_t r_block;
} sponge_context;


uint64_t join_n_bytes(const uint8_t *bytes, size_t n) {
    uint64_t result = 0;
    for (size_t i = 0; i < n; i++) {
        result = (result << 8) + bytes[i];
    }
    return result;
}


// идея заполнения буффера в следующем:
// если оставшаяся длина сообщения позволяет заполнить 8 байт, то мы заполняем буффер 8ми байтами
// если не позволяет, то мы уменьшаем размер буфера до такого значения,
// которого точно хватит для последних блоков размером R_BIT_SIZE.
void fill_buffer(sponge_context *context) {
    size_t buffer_byte_length = 8;

    size_t count_of_remaining_input_bytes = context->message_length - context->bytes_processed;

    if (count_of_remaining_input_bytes >= buffer_byte_length) {
        context->buffer = join_n_bytes(context->message + context->bytes_processed, buffer_byte_length);

        context->bytes_processed += buffer_byte_length;
        context->buffer_remaining_bit_length = buffer_byte_length * 8;
    } else {
        buffer_byte_length = (size_t) ceil((R_BIT_SIZE - context->buffer_remaining_bit_length) / 8.0);

        uint8_t padding_message[buffer_byte_length + 1] = {0x80};
        memcpy(padding_message, context->message + context->bytes_processed, count_of_remaining_input_bytes);
        context->buffer = join_n_bytes(context->message + context->bytes_processed, buffer_byte_length);

        context->buffer = context->buffer >> ((8 - buffer_byte_length) * 8);
        context->bytes_processed = context->message_length;
        context->buffer_remaining_bit_length = R_BIT_SIZE - context->buffer_remaining_bit_length;
    }
}


// заполняет context.r_block и выдает true. Если сообщение кончилось, то false
bool get_r_block(sponge_context *context) {
    if (context->message_length == context->bytes_processed && context->buffer_remaining_bit_length == 0) {
        return false;
    }

    size_t required_count_of_bits;
    if (context->buffer_remaining_bit_length < R_BIT_SIZE) {
        context->r_block = context->buffer << context->buffer_remaining_bit_length;

        required_count_of_bits = R_BIT_SIZE - context->buffer_remaining_bit_length;

        fill_buffer(context);
    } else {
        context->r_block = 0;
        required_count_of_bits = R_BIT_SIZE;
    }
    size_t buffer_shift = context->buffer_remaining_bit_length - required_count_of_bits; // shift for bring R_BLOCK

    context->r_block += context->buffer >> buffer_shift;
    context->buffer = (context->buffer << (64 - buffer_shift)) >> (64 - buffer_shift); // deleting used part
    context->buffer_remaining_bit_length -= required_count_of_bits;
    return true;
}


sponge_struct init_sponge() {
    sponge_struct sponge;
    sponge.state = 0;
    return sponge;
}


sponge_context init_sponge_context(uint8_t *message, size_t message_length) {
    sponge_context context;
    context.message = message;
    context.message_length = message_length;
    context.bytes_processed = 0;
    context.buffer = 0;
    context.buffer_remaining_bit_length = 0;
    context.r_block = 0;

    return context;
}


void sponge_hash(uint8_t *message, size_t length, uint8_t *result) {
    sponge_struct sponge = init_sponge();
    sponge_context context = init_sponge_context(message, length);

    while (get_r_block(&context)) {
        absorbing(&sponge, &context.r_block);
    }

    for(int i = 0; i < 8; i++) {
        result[i] = (sponge.state & (0xFF << (8 - i - 1))) >> (8 - i - 1);
    }
}


void sponge_mac(uint8_t *message, size_t message_length, uint8_t *key, size_t key_length, uint8_t *result) {
    uint8_t *key_with_message = malloc(message_length + key_length);
    memcpy(key_with_message, key, key_length);
    memcpy(key_with_message + key_length, message, message_length);

    sponge_hash(key_with_message, message_length + key_length, result);
    free(key_with_message);
}
