#include <iostream>
#define BYTES_IN_BLOCK 8
#define HALF_BYTES_IN_BLOCK 16

typedef uint8_t bytes_vector[BYTES_IN_BLOCK];
typedef uint8_t half_bytes_vector[HALF_BYTES_IN_BLOCK];

static const uint8_t Pi[HALF_BYTES_IN_BLOCK] = {
        15, 9,  1,  7,
        13, 12, 2,  8,
        6,  5,  14, 3,
        0,  11, 4,  10
};

static const uint8_t Tau[HALF_BYTES_IN_BLOCK] = {
        0, 4, 8,  12,
        1, 5, 9,  13,
        2, 6, 10, 14,
        3, 7, 11, 15
};

static const uint16_t A[HALF_BYTES_IN_BLOCK] = {
        0x3a22, 0x8511, 0x4b99, 0x2cdd,
        0x483b, 0x248c, 0x1246, 0x9123,
        0x59e5, 0xbd7b, 0xcfac, 0x6e56,
        0xac52, 0x56b1, 0xb3c9, 0xc86d
};


void bytes_xor(const uint8_t *first, const uint8_t *second, uint8_t *result) {
    int i;
    for(i = 0; i < HALF_BYTES_IN_BLOCK; i++) {
        result[i] = first[i] ^ second[i];
    }
}


void S(uint8_t *original) {       // Based on Pi, S-box
    int i;
    half_bytes_vector result;
    for (i = 0; i < HALF_BYTES_IN_BLOCK; i++) {
        result[i] = Pi[original[i]];
    }
    memcpy(original, result, HALF_BYTES_IN_BLOCK);
}


void P(uint8_t *original) {  // Based on Tau (half bytes shake)
    int i;
    half_bytes_vector result;
    for (i = 0; i < HALF_BYTES_IN_BLOCK; i++) {
        result[i] = original[Tau[i]];
    }
    memcpy(original, result, HALF_BYTES_IN_BLOCK);
}


uint16_t get_ith_bit(uint16_t x, size_t i)
{
  return x & (1 << (16 - i - 1));
}


uint16_t l_part(uint16_t original) {
    uint16_t result = 0;
    for (size_t i = 0; i < 16; i++) {
        if (get_ith_bit(original, i) >= 1) {
            result ^= A[i];
        }
    }
    return result;
}


void L(uint8_t *original) {   // Linear transformation (matrix_multiply)
    half_bytes_vector result = {0};
    uint16_t bits16 = 0;
    int x, y;
    int i, j;
    for (x = 0; x < 4; x++) {
        for (y = 0; y < 4; y++) {
            bits16 = (bits16 << 4) + original[4 * x + y]; // concat [0xd, 0xe, 0xa, 0xd] => 0xdead = 16 bit
        }
        printf("%x\n", bits16);

        // новое
        uint16_t part_result;
        part_result = l_part(bits16);
        for (i = 0; i < 4; i++) {
            result[4 * x + i] = (part_result & (0xF << (4 * (4 - i -1)))) >> (4 * (4 - i - 1));
        }
//        result[4 * x] = (part_result & 0xF000) >> 12;
//        result[4 * x + 1] = (part_result & 0xF00) >> 8;
//        result[4 * x + 2] = (part_result & 0xF0) >> 4;
//        result[4 * x + 3] = (part_result & 0xF);
//        for (int q = 0; q < 4; q++) printf("result[4 * x + %d] = %x\n", q, result[4 * x + q]);


// прошлое
//        uint8_t current_bit, A_el;
//        for (i = 0; i < 16; i++) {                                      // i = col of A
//            uint8_t result_bit = 0;
//            for (j = 0; j < 16; j++) {                                  // j = row of A and index of bit in vector
////                A_el = (uint8_t)(A[j] << i) >> 15;
////                current_bit = (uint8_t)(bits16 << j) >> 15;
//                A_el = (A[j] & (1 << (15 - i))) >> (15 - i);            // get bit by mask x_bit = (byte & (1 << x)) >> x, x = 0..7
//                current_bit = (bits16 & (1 << (15 - j))) >> (15 - j);
//                result_bit = result_bit ^ current_bit ^ A_el;
//            }
//            result[4 * x + i / 4] = result[4 * x + i / 4] * 2 + result_bit;
//        }
    }
    memcpy(original, result, HALF_BYTES_IN_BLOCK);
}


void split_into_half_bytes(const uint8_t *bytes, uint8_t *result) {
    int i;
    for (i = 0; i < BYTES_IN_BLOCK; i++) {
        result[i * 2] = bytes[i] >> 4;
        result[i * 2 + 1] = bytes[i] & 0xf;
    }
}


void join_half_bytes(const uint8_t *half_bytes, uint8_t *result) {
    int i;
    for (i = 0; i < HALF_BYTES_IN_BLOCK; i++) {
        result[i] = half_bytes[i * 2] * 0xf + half_bytes[i * 2 + 1];
    }
}


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
    bytes_vector a = { 0xf2, 0x52, 0xb7, 0x16, 0x8d, 0x7d, 0x7f, 0x71 };
    half_bytes_vector b;

    split_into_half_bytes(a, b);
//    print_16vector(b);

    S(b);
//    std::cout << "AFTER S\nb = ";
//    print_16vector(b);

    P(b);
    std::cout << "AFTER P\nb = ";
    print_16vector(b);

    L(b);
    std::cout << "AFTER L\nb = ";
    print_16vector(b);
    print_vector_elements(b);
}


int main() {
    test_SPL();
    return 0;
}


//                if (i > 14) {
//                    printf("i = %i\n", i);
//                    printf("j = %i\n", j);
//                    printf("A[j] = %i\n", A[j]);
//                    printf("A_el = %i\n", A_el);
//                    printf("bits16 = %i\n", bits16);
//                    printf("current_bit = %i\n", current_bit);
//                    printf("part result = %i\n", part_result);
//                }
