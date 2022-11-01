#include <iostream>
#include "constants.h"
#include "mora.h"
#include "test_of_parts.h"

void print_8vector(uint8_t *vector) {
    int i;
    for (i = 0; i < 8; i++)
        printf("%x", vector[i]);
    std::cout << "\n";
}

int main() {
//    test_LPS();
//    test_L_part();
//    test_xor();
//    test_adding_in_Z64();
//    test_mora();
    bytes_vector IV = {0};
    uint8_t message[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};
    mora(IV, message, 8);
    print_8vector(IV);
    return 0;
}
