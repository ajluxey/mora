#include <iostream>
#include "mora/constants.h"
#include "mora/mora.h"
#include "mora/test_of_parts.h"
#include "sponge/sponge_crypto.h"

void print_8vector(uint8_t *vector) {
    int i;
    for (i = 0; i < 8; i++)
        printf("%02x ", vector[i]);
    std::cout << "\n";
}


void base_tests() {
    test_LPS();
    test_L_part();
    test_xor();
    test_adding_in_Z64();
}


void mora_run() {
    bytes_vector IV = {0};
    uint8_t message[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};
    mora(IV, message, 8);
    print_8vector(IV);
}


void sponge_hash_run() {
    bytes_vector result = {0};
//    uint8_t message[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};
    uint8_t message[] = {0x87, 0x12, 0xf3, 0x36, 0x52, 0x21, 0x35, 0x59};
    printf("message = ");
    print_8vector(message);

    sponge_hash(message, 8, result);
    print_8vector(result);
}


void sponge_MAC_run() {
    bytes_vector result = {0};
    uint8_t message[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};
    uint8_t key[] = {0x87, 0x12, 0xf3, 0x36, 0x52, 0x21, 0x35, 0x59};
//    sponge_MAC(message, 8, key, 8, result);
    print_8vector(result);
}


int main() {
//    mora_run();
    sponge_hash_run();
//    sponge_MAC_run();
    return 0;
}
