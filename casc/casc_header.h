#ifndef CASC_HEADER_H
#define CASC_HEADER_H

#include <cstdint>

#pragma pack(push, 1)

typedef struct {
    uint32_t header_hash_size;
    uint32_t header_hash;
    uint16_t version;
    uint8_t  bucket_index;
    uint8_t  extra_bytes;
    uint8_t  spec_size;
    uint8_t  spec_offset;
    uint8_t  spec_key;
    uint8_t  spec_offset_bits;
    uint64_t archive_total_size_max;
    uint8_t  padding[8];
    uint32_t entries_size;
    uint32_t entries_hash;
} idx_header;

typedef struct {
    uint8_t  encoding_key[16];
    uint32_t size;
    uint16_t flags;
    uint32_t checksum_a;
    uint32_t checksum_b;
} data_file_header;

#pragma pack(pop)

#endif // CASC_HEADER_H
