#ifndef MORA_MORA_H
#define MORA_MORA_H

#include "constants.h"
#include "byte_utils.h"

void mora(uint8_t *IV, uint8_t *original_message, size_t current_length);

#endif //MORA_MORA_H
