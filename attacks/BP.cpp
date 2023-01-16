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


void restore_message(uint8_t *message, uint32_t seed, uint32_t index, uint64_t range, uint64_t N) {
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


bool check(uint8_t *message_1, uint8_t *message_2) {
    uint8_t constant_part[] = {0x80, 0x00, 0x00, 0x00, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};

    char count = 0;
    for(int i = 0; i < 16; i++) {
        if(message_1[i] == message_2[i]) {
            count++;
        }
    }
    if(count == 16) return false;


    uint8_t test_1[26];
    uint8_t test_2[26];
    memcpy(test_1, message_1, 16);
    memcpy(test_1 + 16, constant_part, 10);
    memcpy(test_2, message_2, 16);
    memcpy(test_2 + 16, constant_part, 10);

    uint8_t sponge_key[] = {0x41, 0x61, 0x42, 0x4f};
    uint8_t result_1[4] = {0};
    uint8_t result_2[4] = {0};

    sponge_mac(test_1, 26, sponge_key, 4, result_1);
    sponge_mac(test_2, 26, sponge_key, 4, result_2);

    //for(int i = 0; i < 4; i++) {
    //    printf("%02x", result_1[i]);
    //}
    //printf("\n");
    //for(int i = 0; i < 4; i++) {
    //    printf("%02x", result_2[i]);
    //}
    //printf("\n");
    for(int i = 0; i < 4; i++) {
        if(result_1[i] != result_2[i]){
            return false;
        }
    }
    return true;
}


void find_collisions(uint32_t flow_index, std::unordered_map<uint32_t, std::vector<uint32_t>> *collision_indexes) {
    char name[4];
    sprintf(name, "%03d", flow_index);

    std::ifstream file(name);
    if(!file.is_open()) {
        printf("Can't open file for flow %d\n", flow_index);
        return;
    }

    uint32_t mac;
    uint32_t index = 0;
    printf("read file\n");
    while(!file.eof()) {
        //printf("new cycle\n");
        file.read(reinterpret_cast<char*>(&mac), sizeof(mac));

        //printf("mac = %08x\n", mac);
        std::vector<uint32_t> tmp = {flow_index, index};
        auto status = collision_indexes->insert(std::make_pair(mac, tmp));
        if (!status.second) {
            (*status.first).second.push_back(flow_index);
            (*status.first).second.push_back(index);
            //printf("collision = %x\n", mac);
            //printf("map already have this mac as key\n");
            //printf("(*status.first).second.at(0) = %d\n", (*status.first).second.at(0));
            //printf("first meeting index is %d\n", (*status.first).second);
            //printf("current index is %d\n", index);
        } //else {
            //printf("Map don't have this mac as key\n");
            //printf("not collision = %x\n", mac);
        //}

        index++;
        file.peek();
    }
}


void check_finded_collisions(const std::unordered_map<uint32_t, std::vector<uint32_t>> &collision_indexes, uint64_t range, uint64_t N) {
    int collisions_count = 0;
    std::array<uint8_t, 16> message = {0};
    std::vector<uint32_t> indexes;
    uint32_t seed, index;

    std::ofstream file("resulted_texts");
    std::vector<std::array<uint8_t, 16>> original_messages;
    for(const auto &col_ind : collision_indexes) {
        indexes = col_ind.second;
        for(int i = 0; i < indexes.size()/2; i++) {
            seed = indexes.at(i*2);
            index = indexes.at(i*2 + 1);
            restore_message(message.data(), seed, index, range, N);
            original_messages.push_back(message);
        }
        for(int i = 0; i < original_messages.size() - 1; i++) {
            for(int j = i+1; j < original_messages.size(); j++) {
                if(check(original_messages.at(i).data(), original_messages.at(j).data())) {
                    file.write((char*)original_messages.at(i).data(), 16);
                    file << std::endl;
                    file.write((char*)original_messages.at(i).data(), 16);
                    file << std::endl << std::endl;
                    collisions_count++;
                }
            }
        }
        original_messages.clear();
        indexes.clear();
//        for(int i = 0; i < (indexes.size() / 2) - 1; i++) {
//            for(int j = i+1; j < (indexes.size() / 2); j++) {
//                seed_1 = indexes.at(i*2);
//                index_1 = indexes.at(i*2 + 1);
//                seed_2 = indexes.at(j*2);
//                index_2 = indexes.at(j*2 + 1);
//
//                restore_message(message_1, seed_1, index_1, range, N);
//                restore_message(message_2, seed_2, index_2, range, N);
//                if (check(message_1, message_2)) {
//                    file.write((char*)message_1, 16);
//                    file << std::endl;
//                    file.write((char*)message_2, 16);
//                    file << std::endl << std::endl;
//                }
//            }
//        }
    }
}


int attack_BP() {
    uint64_t N = 0xffffffff;
    uint64_t range = 0xffffffffffffffff;

//    // generate mac as: mes -> mac_, mes||mac -> mac
    uint64_t n_for_each_flow = N / FLOWS_COUNT;
//    random_N_from_range(range, n_for_each_flow, 0);

    std::vector<std::thread> threads;
//    for(uint32_t i = 0; i < FLOWS_COUNT; i++) {
//        threads.push_back(std::move(std::thread(random_N_from_range, range, n_for_each_flow, i)));
//    }
//    for(auto &th : threads) {
//        th.join();
//    }
//    threads.clear();

    // check collisions
    std::unordered_map<uint32_t, std::vector<uint32_t>> collision_indexes;
    for(uint32_t i = 0; i < FLOWS_COUNT; i++) {
        threads.push_back(std::move(std::thread(find_collisions, i, &collision_indexes)));
    }
    for(auto &th : threads) {
        th.join();
    }
    check_finded_collisions(collision_indexes, range, n_for_each_flow);
    collision_indexes.clear();

//    uint64_t index_1 = 1680385;
//    uint64_t index_2 = 1816656;
//    uint8_t message_1[16] = {0};
//    uint8_t message_2[16] = {0};
//    restore_message(message_1, 0, index_1, range, n_for_each_flow);
//    restore_message(message_2, 0, index_2, range, n_for_each_flow);
//
//    print_vector_attack(message_1);
//    print_vector_attack(message_2);
//
//    check(message_1, message_2);
    return 0;
}
