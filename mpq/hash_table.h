#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <cstdint>

#define HASH_ENTRY_EMPTY   0xFFFFFFFF
#define HASH_ENTRY_DELETED 0xFFFFFFFE

typedef struct {
    uint32_t hash_a;
    uint32_t hash_b;
    uint16_t locale;
    uint16_t platform;
    uint32_t block_index;
} hash_table_entry;

int32_t HashTableLookup(const char* filename, const hash_table_entry* hash_table,
                        uint32_t table_size, const uint32_t* crypt_table);

#endif // HASH_TABLE_H
