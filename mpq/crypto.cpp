#include "crypto.h"
#include <cctype>

void InitCryptTable(uint32_t* crypt_table) {
    uint32_t seed = 0x00100001;

    for (uint32_t index1 = 0; index1 < 0x100; index1++) {
        uint32_t index2 = index1;
        for (int i = 0; i < 5; i++) {
            seed = (seed * 125 + 3) % 0x2AAAAB;
            uint32_t temp1 = (seed & 0xFFFF) << 0x10;

            seed = (seed * 125 + 3) % 0x2AAAAB;
            uint32_t temp2 = seed & 0xFFFF;

            crypt_table[index2] = temp1 | temp2;
            index2 += 0x100;
        }
    }
}

uint32_t HashString(const char* key, uint32_t hash_type, const uint32_t* crypt_table) {
    uint32_t seed1 = 0x7FED7FED;
    uint32_t seed2 = 0xEEEEEEEE;

    while (*key) {
        unsigned char ch = (unsigned char)toupper((unsigned char)*key++);
        seed1 = crypt_table[hash_type + ch] ^ (seed1 + seed2);
        seed2 = ch + seed1 + seed2 + (seed2 << 5) + 3;
    }

    return seed1;
}

void DecryptBlock(uint32_t* data, uint32_t dword_count, uint32_t key, const uint32_t* crypt_table) {
    uint32_t seed = 0xEEEEEEEE;

    for (uint32_t i = 0; i < dword_count; i++) {
        seed += crypt_table[0x400 + (key & 0xFF)];
        uint32_t ch = data[i] ^ (key + seed);
        key = ((~key << 0x15) + 0x11111111) | (key >> 0x0B);
        seed = ch + seed + (seed << 5) + 3;
        data[i] = ch;
    }
}
