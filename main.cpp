#include <iostream>
#include "constants.h"
#include "LPS.h"
#include "byte_utils.h"


void print_16vector(uint8_t *vector) {
    int i;
    for (i = 0; i < 16; i++)
        printf("%x", vector[i]);
    std::cout << "\n";
}

void print_vector_elements(uint8_t *vector) {
    int i;
    for (i = 0; i < 16; i++)
        printf("a[%i] = %x\n", i, vector[i]);
    std::cout << "\n";
}


int test_LPS() {
    bytes_vector a = { 0xf2, 0x52, 0xb7, 0x16, 0x8d, 0x7d, 0x7f, 0x71 };
    half_bytes_vector expected = {0x2, 0xc, 0x5, 0xb, 0x8, 0x3, 0x6, 0x2, 0x9, 0x3, 0x9, 0xc, 0xa, 0x2, 0xb, 0xb};
    half_bytes_vector result;

    split_into_half_bytes(a, result);
    print_16vector(result);

    S(result);
//    std::cout << "AFTER S\nb = ";
//    print_16vector(result);

    P(result);
//    std::cout << "AFTER P\nb = ";
//    print_16vector(result);

    L(result);
//    std::cout << "AFTER L\nb = ";
//    print_16vector(result);

    for(int i = 0; i < HALF_BYTES_IN_BLOCK; i++) {
        if (result[i] != expected[i]) {
            printf("FAIL!");
            return -1;
        }
    }
    printf("CORR!");
    return 1;
}


int test_L_part() {
    uint16_t r1 = 0xa368;
    uint16_t r2 = 0x18ba;
    uint16_t e1 = 0x2c5b;
    uint16_t e2 = 0x8362;
    if (l_part(r1) == e1 && l_part(r2) == e2) {
        printf("CORR!");
        return 1;
    } else {
        printf("FAIL!");
        return -1;
    }
}


int test_xor() {
    half_bytes_vector a = {0x2, 0xf, 0x0, 0x3, 0x8, 0xf, 0x3, 0xa, 0xf, 0x4, 0xb, 0x8, 0x9, 0x5, 0xd, 0xf};
    half_bytes_vector b = {0x0, 0x6, 0xf, 0xb, 0xc, 0x6, 0xa, 0x6, 0x9, 0xc, 0x3, 0xf, 0x9, 0x4, 0xc, 0xc};
    half_bytes_vector result;
    half_bytes_vector expected = {0x2, 0x9, 0xf, 0x8, 0x4, 0x9, 0x9, 0xc, 0x6, 0x8, 0x8, 0x7, 0x0, 0x1, 0x1, 0x3};
    bytes_xor(a, b, result);
    for (int i = 0; i < HALF_BYTES_IN_BLOCK; i++) {
        if (result[i] != expected[i]) {
            printf("FAIL!");
            return -1;
        }
    }
    printf("CORR!");
    return 1;
}


int main() {
    test_LPS();
//    test_L_part();
//    test_xor();
    return 0;
}
