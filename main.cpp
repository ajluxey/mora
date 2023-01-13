#include <iostream>
#include "mora/constants.h"
#include "mora/mora.h"
#include "mora/test_of_parts.h"
#include "sponge/sponge_crypto.h"
#include <ctime>
#include "attacks/BP.h"


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
//    uint8_t message[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};
    uint8_t message[] = {0x87, 0x12, 0xf3, 0x36, 0x52, 0x21, 0x35, 0x59};
    bytes_vector result = {0};

    printf("message = ");
    print_8vector(message);

    mora(IV, message, 8, result);
    printf("hash    = ");
    print_8vector(result);
}

void sponge_hash_run() {
//    uint8_t message[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};
    uint8_t message[] = {0x87, 0x12, 0xf3, 0x36, 0x52, 0x21, 0x35, 0x59};
    bytes_vector result = {0};

    printf("message = ");
    print_8vector(message);

    sponge_hash(message, 8, result);
    printf("hash    = ");
    print_8vector(result);
}


void sponge_MAC_run() {
    bytes_vector result = {0};
    uint8_t message[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};
    uint8_t key[] = {0x87, 0x12, 0xf3, 0x36, 0x52, 0x21, 0x35, 0x59};
    sponge_mac(message, 8, key, 8, result);
    print_8vector(result);
}



void time_test(size_t exec_times) {
    double time_spent = 0.0;
    uint8_t message[] = {0x87, 0x12, 0xf3, 0x36, 0x52, 0x21, 0x35, 0x59};
    bytes_vector result = {0};

    // Mora
    bytes_vector IV = {0};

    clock_t begin = clock();
    for(size_t i = 0; i < exec_times; i++) {
        mora(IV, message, 8, result);
    }
    clock_t end = clock();
    time_spent += (double)(end - begin) / CLOCKS_PER_SEC;

    printf("Mora:\n");
    printf("message = ");
    print_8vector(message);
    printf("hash = ");
    print_8vector(result);
    printf("Execs %d times for %f s\n", exec_times, time_spent);

    // Sponge
    time_spent = 0.0;

    begin = clock();
    for(size_t i = 0; i < exec_times; i++) {
        sponge_hash(message, 8, result);
    }
    end = clock();
    time_spent += (double)(end - begin) / CLOCKS_PER_SEC;

    printf("\nSponge (|S| = %d, |Sr| = %d):\n", STATE_BIT_SIZE, R_BIT_SIZE);
    printf("message = ");
    print_8vector(message);
    printf("hash = ");
    print_8vector(result);
    printf("Execs %d times for %f s\n", exec_times, time_spent);

}



int main() {
//    mora_run();
//    sponge_hash_run();
//    sponge_MAC_run();
//    time_test(1000000);
    attack_BP();
    return 0;
}
