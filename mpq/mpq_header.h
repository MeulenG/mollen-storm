#ifndef MPQ_HEADER_H
#define MPQ_HEADER_H

#include <cstdint>

typedef struct {
    char     magic[4];
    uint32_t header_size;
    uint32_t archive_size;
    uint16_t format_version;
    uint16_t block_size;
    uint32_t hash_table_pos;
    uint32_t block_table_pos;
    uint32_t hash_table_size;
    uint32_t block_table_size;
} mpq_header_v1;

typedef struct {
    char     magic[4];
    uint32_t header_size;
    uint32_t archive_size;
    uint16_t format_version;
    uint16_t block_size;
    uint32_t hash_table_pos;
    uint32_t block_table_pos;
    uint32_t hash_table_size;
    uint32_t block_table_size;
    uint64_t hi_block_table_pos;
    uint16_t hash_table_pos_hi;
    uint16_t block_table_pos_hi;
} mpq_header_v2;

#endif // MPQ_HEADER_H
