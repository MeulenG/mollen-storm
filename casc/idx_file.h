#ifndef IDX_FILE_H
#define IDX_FILE_H

#include <cstdint>
#include <vector>
#include "casc_header.h"

struct idx_entry {
    uint8_t  key[16];
    uint32_t archive_number;
    uint32_t archive_offset;
    uint32_t size;
};

uint8_t GetBucketIndex(const uint8_t key[9]);

class IdxFile {
public:
    bool Load(const char* path);
    const idx_entry* Lookup(const uint8_t* key, uint8_t key_length) const;

private:
    idx_header header_;
    std::vector<idx_entry> entries_;
};

#endif // IDX_FILE_H
