#include <sstream>
#include <thread>
#include "BP.h"

#define FLOWS_COUNT 2
uint8_t MAC_count[0xffffffff] = {0};


void print_vector_attack(uint8_t *array) {
    for(int i = 0; i < 12; i++) {
        printf("%02x", array[i]);
    }
    printf("\n");
}


void decimal_to_array(uint64_t decimal, uint8_t *array) {
    uint64_t previous = decimal;

    for(int i = 0; i < 8; i++){
        array[7-i] = previous % 0x100;
        previous = previous / 0x100;
    }
}


uint32_t mac_to_decimal(uint8_t *mac) {
    uint32_t result = 0;
    for(int i = 0; i < 4; i++) {
        result *= 0x100;
        result += mac[i];
    }
    return result;
}


void random_N_from_range(uint64_t range, uint64_t N, int flow_index) {
    char name[3];
    sprintf(name, "%03d", flow_index);
    printf("%s\n", name);
//    FILE *file;
//    if ((file = fopen(name, "w")) == NULL) {
//        printf("Can't open file for flow %d\n", flow_index);
//        return;
//    }

    uint64_t left_border = 0;
    uint64_t mini_range = ceil(range / N);

    std::mt19937 gen(flow_index);
    std::uniform_int_distribution<uint64_t> random_from_diapazon(0, mini_range);

    uint64_t uniq;
    uint8_t sponge_key[] = {0x41, 0x61, 0x42, 0x4f};
    uint8_t sponge_message[8] = {0};
    uint8_t sponge_message_with_mac[12] = {0};
    uint8_t potential_collision[4] = {0};

    uint64_t i = 0;
    for(i; i < N; i++) {
        uniq = left_border + random_from_diapazon(gen) % (range % N);

        decimal_to_array(uniq, sponge_message);
        sponge_mac(sponge_message, 8, sponge_key, 4, sponge_message_with_mac + 8);
        memcpy(sponge_message_with_mac, sponge_message, 8);
        sponge_mac(sponge_message_with_mac, 12, sponge_key, 4, potential_collision);

        for(int j = 0; j < 4; j++){
            printf("%c", potential_collision[j]);
//            fputc(potential_collision[j], file);
        }
        printf("\n");
//        fputs("\n", file);

        left_border += mini_range;
    }
//    fclose(file);
}

int attack_BP() {
    uint64_t N = 100;
    uint64_t range = 0xffffffffffffffff;

    uint64_t n_for_each_flow = N / FLOWS_COUNT;

    std::vector<std::thread> threads;
    for(int i = 0; i < FLOWS_COUNT; i++) {
        threads.push_back(std::thread(random_N_from_range, range, n_for_each_flow, i));
//        random_N_from_range(range, n_for_each_flow, i);
    }
    for(auto &th : threads) {
        th.join();
    }
    return 0;
}


//    FILE* file;
//    if ((file = fopen(ctime(&seed), "w")) == NULL) {
//        printf("Can't open file %s\n", ctime(&seed));
//        return 0;
//    } else {
//        fputc(seed, file);
//        fputs("\n",file);
//    }
//    fclose(file);


//        printf("%016llx -> ", uniq);
//        print_vector_attack(sponge_message);
//        print_vector_attack(sponge_message_with_mac);
