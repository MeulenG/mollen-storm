#include "mpq_archive.h"
#include "file_flags.h"
#include "compression.h"
#include <cstring>

MpqArchive::MpqArchive()
    : file_(nullptr), header_{}, archive_offset_(0), sector_size_(0) {
    InitCryptTable(crypt_table_);
}

MpqArchive::~MpqArchive() {
    Close();
}

bool MpqArchive::Open(const char* path) {
    Close();

    file_ = fopen(path, "rb");
    if (!file_) {
        return false;
    }

    if (!ReadHeader()) {
        Close();
        return false;
    }

    if (!ReadHashTable()) {
        Close();
        return false;
    }

    if (!ReadBlockTable()) {
        Close();
        return false;
    }

    return true;
}

void MpqArchive::Close() {
    if (file_) {
        fclose(file_);
        file_ = nullptr;
    }
    hash_table_.clear();
    block_table_.clear();
    sector_size_ = 0;
}

bool MpqArchive::ReadHeader() {
    fseek(file_, (long)archive_offset_, SEEK_SET);
    if (fread(&header_, sizeof(mpq_header_v1), 1, file_) != 1) {
        return false;
    }

    if (memcmp(header_.magic, "MPQ\x1A", 4) != 0) {
        return false;
    }

    sector_size_ = 512u << header_.block_size;
    return true;
}

bool MpqArchive::ReadHashTable() {
    hash_table_.resize(header_.hash_table_size);

    fseek(file_, (long)(archive_offset_ + header_.hash_table_pos), SEEK_SET);
    if (fread(hash_table_.data(), sizeof(hash_table_entry),
              header_.hash_table_size, file_) != header_.hash_table_size) {
        return false;
    }

    uint32_t key = HashString("(hash table)", HASH_TYPE_FILE_KEY, crypt_table_);
    DecryptBlock(reinterpret_cast<uint32_t*>(hash_table_.data()),
                 (header_.hash_table_size * sizeof(hash_table_entry)) / sizeof(uint32_t),
                 key, crypt_table_);

    return true;
}

bool MpqArchive::ReadBlockTable() {
    block_table_.resize(header_.block_table_size);

    fseek(file_, (long)(archive_offset_ + header_.block_table_pos), SEEK_SET);
    if (fread(block_table_.data(), sizeof(block_table_entry),
              header_.block_table_size, file_) != header_.block_table_size) {
        return false;
    }

    uint32_t key = HashString("(block table)", HASH_TYPE_FILE_KEY, crypt_table_);
    DecryptBlock(reinterpret_cast<uint32_t*>(block_table_.data()),
                 (header_.block_table_size * sizeof(block_table_entry)) / sizeof(uint32_t),
                 key, crypt_table_);

    return true;
}

std::vector<uint8_t> MpqArchive::ExtractFile(const char* filename) {
    int32_t block_index = HashTableLookup(filename, hash_table_.data(),
                                          header_.hash_table_size, crypt_table_);
    if (block_index < 0 || (uint32_t)block_index >= header_.block_table_size) {
        return {};
    }

    const block_table_entry& block = block_table_[block_index];
    if (!(block.flags & MPQ_FILE_EXISTS)) {
        return {};
    }

    uint32_t file_key = 0;
    if (block.flags & MPQ_FILE_ENCRYPTED) {
        file_key = GetFileKey(filename, block);
    }

    if (block.flags & MPQ_FILE_SINGLE_UNIT) {
        return ExtractSingleUnit(block, file_key);
    }

    return ExtractSectorBased(block, file_key);
}

uint32_t MpqArchive::GetFileKey(const char* filename, const block_table_entry& block) {
    const char* base = strrchr(filename, '\\');
    if (!base) {
        base = strrchr(filename, '/');
    }
    const char* key_name = base ? base + 1 : filename;

    uint32_t key = HashString(key_name, HASH_TYPE_FILE_KEY, crypt_table_);

    if (block.flags & MPQ_FILE_FIX_KEY) {
        key = (key + block.file_offset) ^ block.uncompressed_size;
    }

    return key;
}

std::vector<uint8_t> MpqArchive::ExtractSingleUnit(const block_table_entry& block, uint32_t file_key) {
    fseek(file_, (long)(archive_offset_ + block.file_offset), SEEK_SET);

    std::vector<uint8_t> raw(block.compressed_size);
    if (fread(raw.data(), 1, block.compressed_size, file_) != block.compressed_size) {
        return {};
    }

    if (block.flags & MPQ_FILE_ENCRYPTED) {
        DecryptBlock(reinterpret_cast<uint32_t*>(raw.data()),
                     block.compressed_size / sizeof(uint32_t),
                     file_key, crypt_table_);
    }

    if (block.compressed_size < block.uncompressed_size) {
        std::vector<uint8_t> output(block.uncompressed_size);
        if (!DecompressSector(raw.data(), block.compressed_size,
                              output.data(), block.uncompressed_size, block.flags)) {
            return {};
        }
        return output;
    }

    return raw;
}

std::vector<uint8_t> MpqArchive::ExtractSectorBased(const block_table_entry& block, uint32_t file_key) {
    uint32_t sector_count = (block.uncompressed_size + sector_size_ - 1) / sector_size_;
    uint32_t offset_count = sector_count + 1;
    if (block.flags & MPQ_FILE_SECTOR_CRC) {
        offset_count++;
    }

    fseek(file_, (long)(archive_offset_ + block.file_offset), SEEK_SET);

    std::vector<uint32_t> sector_offsets(offset_count);
    if (fread(sector_offsets.data(), sizeof(uint32_t), offset_count, file_) != offset_count) {
        return {};
    }

    if (block.flags & MPQ_FILE_ENCRYPTED) {
        DecryptBlock(sector_offsets.data(), offset_count, file_key - 1, crypt_table_);
    }

    std::vector<uint8_t> result(block.uncompressed_size);
    uint32_t bytes_written = 0;

    for (uint32_t i = 0; i < sector_count; i++) {
        uint32_t sector_compressed_size = sector_offsets[i + 1] - sector_offsets[i];
        uint32_t sector_uncompressed_size = sector_size_;
        if (i == sector_count - 1) {
            sector_uncompressed_size = block.uncompressed_size - (sector_size_ * i);
        }

        fseek(file_, (long)(archive_offset_ + block.file_offset + sector_offsets[i]), SEEK_SET);

        std::vector<uint8_t> sector_data(sector_compressed_size);
        if (fread(sector_data.data(), 1, sector_compressed_size, file_) != sector_compressed_size) {
            return {};
        }

        if (block.flags & MPQ_FILE_ENCRYPTED) {
            DecryptBlock(reinterpret_cast<uint32_t*>(sector_data.data()),
                         sector_compressed_size / sizeof(uint32_t),
                         file_key + i, crypt_table_);
        }

        if (sector_compressed_size < sector_uncompressed_size) {
            if (!DecompressSector(sector_data.data(), sector_compressed_size,
                                  result.data() + bytes_written, sector_uncompressed_size,
                                  block.flags)) {
                return {};
            }
        } else {
            memcpy(result.data() + bytes_written, sector_data.data(), sector_uncompressed_size);
        }

        bytes_written += sector_uncompressed_size;
    }

    return result;
}

bool MpqArchive::DecompressSector(const uint8_t* input, uint32_t input_size,
                                  uint8_t* output, uint32_t output_size, uint32_t flags) {
    if (flags & MPQ_FILE_COMPRESS) {
        uint8_t comp_type = input[0];
        return Decompress(comp_type, input + 1, input_size - 1, output, output_size);
    }

    if (flags & MPQ_FILE_IMPLODE) {
        return Decompress(COMPRESSION_PKWARE, input, input_size, output, output_size);
    }

    return false;
}
