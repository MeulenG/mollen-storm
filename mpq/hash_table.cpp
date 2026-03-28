#include "hash_table.h"
#include "crypto.h"

int32_t HashTableLookup(const char* filename, const hash_table_entry* hash_table,
                        uint32_t table_size, const uint32_t* crypt_table) {
    if (table_size == 0) {
        return -1;
    }

    uint32_t start = HashString(filename, HASH_TYPE_TABLE_OFFSET, crypt_table) % table_size;
    uint32_t hash_a = HashString(filename, HASH_TYPE_NAME_A, crypt_table);
    uint32_t hash_b = HashString(filename, HASH_TYPE_NAME_B, crypt_table);

    uint32_t index = start;
    do {
        const hash_table_entry& entry = hash_table[index];

        if (entry.block_index == HASH_ENTRY_EMPTY) {
            return -1;
        }

        if (entry.block_index != HASH_ENTRY_DELETED &&
            entry.hash_a == hash_a && entry.hash_b == hash_b) {
            return (int32_t)entry.block_index;
        }

        index = (index + 1) % table_size;
    } while (index != start);

    return -1;
}
