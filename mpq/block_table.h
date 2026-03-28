#ifndef BLOCK_TABLE_H
#define BLOCK_TABLE_H

#include <cstdint>

typedef struct {
    uint32_t file_offset;
    uint32_t compressed_size;
    uint32_t uncompressed_size;
    uint32_t flags;
} block_table_entry;

#endif // BLOCK_TABLE_H
