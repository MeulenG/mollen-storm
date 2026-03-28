#ifndef MPQ_ARCHIVE_H
#define MPQ_ARCHIVE_H

#include <cstdint>
#include <cstdio>
#include <vector>

#include "crypto.h"
#include "mpq_header.h"
#include "hash_table.h"
#include "block_table.h"

class MpqArchive {
public:
    MpqArchive();
    ~MpqArchive();

    bool Open(const char* path);
    void Close();
    std::vector<uint8_t> ExtractFile(const char* filename);

private:
    FILE* file_;
    mpq_header_v1 header_;
    std::vector<hash_table_entry> hash_table_;
    std::vector<block_table_entry> block_table_;
    uint32_t crypt_table_[CRYPT_TABLE_SIZE];
    uint32_t archive_offset_;
    uint32_t sector_size_;

    bool ReadHeader();
    bool ReadHashTable();
    bool ReadBlockTable();

    uint32_t GetFileKey(const char* filename, const block_table_entry& block);
    std::vector<uint8_t> ExtractSingleUnit(const block_table_entry& block, uint32_t file_key);
    std::vector<uint8_t> ExtractSectorBased(const block_table_entry& block, uint32_t file_key);
    bool DecompressSector(const uint8_t* input, uint32_t input_size,
                          uint8_t* output, uint32_t output_size, uint32_t flags);
};

#endif // MPQ_ARCHIVE_H
