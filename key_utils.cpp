#include "key_utils.h"

unsigned long long K(uint8_t *vector_1, uint8_t *vector_2) {
    half_bytes_vector result;
    bytes_xor(vector_1, vector_2, result);
    S(result);
    P(result);
    L(result);
}