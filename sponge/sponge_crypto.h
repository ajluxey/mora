//
// Created by Alexey on 05.11.2022.
//

#ifndef MORA_SPONGE_CRYPTO_H
#define MORA_SPONGE_CRYPTO_H

#include "sponge_construction.h"

void sponge_hash(uint8_t *message, size_t length, uint8_t *result);
void sponge_mac(uint8_t *message, size_t message_length, uint8_t *key, size_t key_length, uint8_t *result);

#endif //MORA_SPONGE_CRYPTO_H
