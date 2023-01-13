#include "sponge_crypto.h"


typedef struct sponge_context {
    uint8_t *message;
    size_t message_length;
    bool message_was_padded;
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
    size_t remaining_input_bytes_length = context->message_length - context->bytes_processed;

    if (remaining_input_bytes_length >= buffer_byte_length) {
        context->buffer = join_n_bytes(context->message + context->bytes_processed, buffer_byte_length);
        context->buffer_remaining_bit_length = buffer_byte_length * 8;
        context->bytes_processed += buffer_byte_length;
    } else {
        size_t message_bit_length = context->message_length * 8;
        size_t needed_count_of_r_blocks_in_message = message_bit_length / R_BIT_SIZE + 1;               // даже если сообщение полностью делится на блоки, то нужен паддинг, поэтому + 1
        size_t padding_message_byte_length = ceil(double(needed_count_of_r_blocks_in_message) * R_BIT_SIZE / 8);
        size_t count_of_extra_zeros = padding_message_byte_length * 8 - needed_count_of_r_blocks_in_message * R_BIT_SIZE;
        buffer_byte_length = padding_message_byte_length - context->bytes_processed;                    // буффер вмещающий полный остаток сообщения + необходимое количество байт для полного блока R (с возможным запасом, который обрежется)

        // padding
        uint8_t *padding_buffer = static_cast<uint8_t *>(malloc(buffer_byte_length));
        memset(padding_buffer, 0x0, buffer_byte_length);
        memcpy(padding_buffer, context->message + context->bytes_processed, remaining_input_bytes_length);
        padding_buffer[context->message_length - context->bytes_processed] = 0x80;

        context->buffer = join_n_bytes(padding_buffer, buffer_byte_length) << (8 - buffer_byte_length) * 8;
        context->buffer >>= count_of_extra_zeros;                                                       // срезаем хвост из нулей
        free(padding_buffer);
        context->bytes_processed = context->message_length;
        context->buffer_remaining_bit_length = buffer_byte_length * 8 - count_of_extra_zeros;
        context->message_was_padded = true;
    }
}


// заполняет context.r_block и выдает true. Если сообщение кончилось, то false
bool get_r_block(sponge_context *context) {
    if (context->message_length == context->bytes_processed && context->buffer_remaining_bit_length == 0 && context->message_was_padded) {
        return false;
    }
    size_t required_count_of_bits = R_BIT_SIZE;
    context->r_block = 0;

    if (context->buffer_remaining_bit_length < R_BIT_SIZE) {
        required_count_of_bits = R_BIT_SIZE - context->buffer_remaining_bit_length;
        context->r_block = context->buffer << context->buffer_remaining_bit_length;     // если длины буффера не хватает, сохраняет его часть и заполняет новый

        fill_buffer(context);
    }
    context->r_block += (context->buffer >> (64 - required_count_of_bits));
    context->buffer = (context->buffer << (64 - required_count_of_bits));
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
    context.message_was_padded = false;
    context.bytes_processed = 0;
    context.buffer = 0;
    context.buffer_remaining_bit_length = 0;
    context.r_block = 0;

    return context;
}


void sponge_hash(uint8_t *message, size_t length, uint8_t *result) {
    sponge_struct sponge = init_sponge();
    sponge_context context = init_sponge_context(message, length);

    // absorbing all message
    while (get_r_block(&context)) {
        absorbing(&sponge, &context.r_block);
    }

    for(int i = 0; i < 4; i++) {
        result[i] = (sponge.state & (0xFFULL << (8 - i - 1)*8)) >> (8 - i - 1) * 8;
    }
}


void sponge_mac(uint8_t *message, size_t message_length, uint8_t *key, size_t key_length, uint8_t *result) {
    uint8_t *key_with_message = static_cast<uint8_t *>(malloc(message_length + key_length));
    memcpy(key_with_message, key, key_length);
    memcpy(key_with_message + key_length, message, message_length);

    sponge_hash(key_with_message, message_length + key_length, result);
    free(key_with_message);
}
