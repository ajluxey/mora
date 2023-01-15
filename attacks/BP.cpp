#include <fstream>
#include "BP.h"

#define FLOWS_COUNT 64


void print_vector_attack(uint8_t *array) {
    for(int i = 0; i < 16; i++) {
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
    char name[4];
    sprintf(name, "%03d", flow_index);
    printf("%s\n", name);

    std::ofstream file(name);
    if(!file.is_open()) {
        printf("Can't open file for flow %d\n", flow_index);
        return;
    }

    uint64_t left_border = 0;
    uint64_t mini_range = ceil(range / N);

    std::mt19937_64 gen(flow_index);
    std::uniform_int_distribution<uint64_t> random_from_diapazon(0, mini_range);

    uint64_t uniq;
    uint8_t sponge_key[] = {0x41, 0x61, 0x42, 0x4f};
    uint8_t sponge_message_with_mac[16] = {0};
    uint8_t potential_collision[4] = {0};
    memset(sponge_message_with_mac + 8, 0x80, 1);

    for(uint64_t i = 0; i < N; i++) {
        uniq = left_border + random_from_diapazon(gen) % (range % N);

        decimal_to_array(uniq, sponge_message_with_mac);
        sponge_mac(sponge_message_with_mac, 8, sponge_key, 4, sponge_message_with_mac + 12);
        sponge_mac(sponge_message_with_mac, 16, sponge_key, 4, potential_collision);

        file.write((char*)potential_collision, 4);
        left_border += mini_range;
    }
}


void check_collisions(uint32_t flow_index) {
    char name[4];
    sprintf(name, "%03d", flow_index);

    std::ifstream file(name);
    if(!file.is_open()) {
        printf("Can't open file for flow %d\n", flow_index);
        return;
    }

    std::unordered_map<uint32_t, uint32_t> first_meeting_index;

    uint8_t *mac_bytes;
    char char_mac[4];
    uint32_t mac;
    uint32_t index = 0;
    while(!file.eof()) {
        file.read(reinterpret_cast<char*>(&mac), sizeof(mac));

        auto status = first_meeting_index.insert(std::make_pair(mac, index));
        if (!status.second) {
            printf("collision = %x\n", mac);
            printf("first meeting index is %d\n", (*status.first).second);
        } else {
            printf("not collision = %x\n", mac);
        }

        index++;
        file.peek();
    }
    first_meeting_index.clear();
}


void restore_message(uint8_t *message, int seed, uint32_t index, uint64_t range, uint64_t N) {
    memset(message + 8, 0x80, 1);

    uint64_t left_border = 0;
    uint64_t mini_range = ceil(range / N);

    std::mt19937_64 gen(seed);
    std::uniform_int_distribution<uint64_t> random_from_diapazon(0, mini_range);

    uint64_t uniq;
    uint8_t sponge_key[] = {0x41, 0x61, 0x42, 0x4f};

    gen.discard(index);
    left_border += mini_range * (index);
    uniq = left_border + random_from_diapazon(gen) % (range % N);
    decimal_to_array(uniq, message);
    sponge_mac(message, 8, sponge_key, 4, message + 12);
}


void generate_two_and_check(uint8_t *message_1, uint8_t *message_2) {
    uint8_t constant_part[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};

    uint8_t test_1[26];
    uint8_t test_2[26];
    memcpy(test_1, message_1, 12);
    memcpy(test_1 + 16, constant_part, 10);
    memcpy(test_2, message_2, 12);
    memcpy(test_2 + 16, constant_part, 10);

    uint8_t sponge_key[] = {0x41, 0x61, 0x42, 0x4f};
    uint8_t result_1[4] = {0};
    uint8_t result_2[4] = {0};

    sponge_mac(message_1, 22, sponge_key, 4, result_1);
    sponge_mac(message_2, 22, sponge_key, 4, result_2);

    for(int i = 0; i < 4; i++) {
        if(result_1[i] != result_2[i]){
            printf("gavno\n");
        }
    }
    printf("check completed\n");
}


int attack_BP() {
    uint64_t N = 0xffffffff;
    uint64_t range = 0xffffffffffffffff;

//    // generate mac as: mes -> mac_, mes||mac -> mac
    uint64_t n_for_each_flow = N / FLOWS_COUNT;
////    random_N_from_range(range, n_for_each_flow, 0);
//
//    std::vector<std::thread> threads;
//    for(int i = 0; i < 1; i++) {//FLOWS_COUNT; i++) {
//        threads.push_back(std::thread(random_N_from_range, range, n_for_each_flow, i));
//    }
//    for(auto &th : threads) {
//        th.join();
//    }

    // check collisions
//    for(int i = 0; i < FLOWS_COUNT; i++) {
//    }

    // check attack
    uint64_t index_1 = 0;
    uint64_t index_2 = 1;
    uint8_t message_1[16] = {0};
    uint8_t message_2[16] = {0};
    restore_message(message_1, 0, index_1, range, n_for_each_flow);
    restore_message(message_2, 0, index_2, range, n_for_each_flow);

    print_vector_attack(message_1);
    print_vector_attack(message_2);

//    generate_two_and_check(message_1, message_2);
    return 0;
}

//        printf("%016llx -> ", uniq);
//        print_vector_attack(sponge_message);
//        print_vector_attack(sponge_message_with_mac);
