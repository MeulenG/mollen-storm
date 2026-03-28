#ifndef MPQ_ARCHIVE_H
#define MPQ_ARCHIVE_H

#include <cstdint>
#include <cstdio>
#include <string>
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
    std::vector<std::string> GetListFile();

private:
    FILE* file_;
    mpq_header_v1 header_;
    uint64_t hi_block_table_pos_;
    uint16_t hash_table_pos_hi_;
    uint16_t block_table_pos_hi_;
    std::vector<hash_table_entry> hash_table_;
    std::vector<block_table_entry> block_table_;
    std::vector<uint16_t> hi_block_table_;
    uint32_t crypt_table_[CRYPT_TABLE_SIZE];
    uint64_t archive_offset_;
    uint32_t sector_size_;

    bool ReadHeader();
    bool ReadHashTable();
    bool ReadBlockTable();
    bool ReadHiBlockTable();

    uint64_t GetHashTableOffset();
    uint64_t GetBlockTableOffset();
    uint64_t GetFileOffset(uint32_t block_index);

    uint32_t GetFileKey(const char* filename, uint32_t block_index);
    std::vector<uint8_t> ExtractSingleUnit(const block_table_entry& block,
                                           uint32_t file_key, uint64_t file_offset);
    std::vector<uint8_t> ExtractSectorBased(const block_table_entry& block,
                                            uint32_t file_key, uint64_t file_offset);
    bool DecompressSector(const uint8_t* input, uint32_t input_size,
                          uint8_t* output, uint32_t output_size, uint32_t flags);

    void Seek(uint64_t offset);
};

#endif // MPQ_ARCHIVE_H
