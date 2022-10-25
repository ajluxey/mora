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


void test_SPL() {
//    bytes_vector a = { 0xf2, 0x52, 0xb7, 0x16, 0x8d, 0x7d, 0x7f, 0x71 };
    bytes_vector a = {};
    half_bytes_vector b;

    split_into_half_bytes(a, b);
    print_16vector(b);

    S(b);
    std::cout << "AFTER S\nb = ";
    print_16vector(b);

    P(b);
    std::cout << "AFTER P\nb = ";
    print_16vector(b);

    L(b);
    std::cout << "AFTER L\nb = ";
    print_16vector(b);
}


void test_unit() {
    uint16_t t1 = 0xa368;
    uint16_t t2 = 0x18ba;
// 	printf("%x\n", l_part_result);
    printf("%x\n", l_part(t2));
}


int main() {
    test_SPL();
    // test_unit();
    return 0;
}
