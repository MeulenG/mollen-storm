#include "idx_file.h"
#include <cstring>

uint8_t GetBucketIndex(const uint8_t key[9]) {
    uint8_t i = key[0] ^ key[1] ^ key[2] ^ key[3] ^
                key[4] ^ key[5] ^ key[6] ^ key[7] ^ key[8];
    return (i & 0x0F) ^ (i >> 4);
}

bool IdxFile::Load(const char* path) {
    (void)path;
    return false;
}

const idx_entry* IdxFile::Lookup(const uint8_t* key, uint8_t key_length) const {
    (void)key;
    (void)key_length;
    return nullptr;
}
