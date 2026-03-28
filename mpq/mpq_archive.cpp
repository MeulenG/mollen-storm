#include "mpq_archive.h"
#include "file_flags.h"
#include "compression.h"
#include <cstring>

MpqArchive::MpqArchive()
    : file_(nullptr), header_{}, archive_offset_(0) {
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
}

bool MpqArchive::ReadHeader() {
    fseek(file_, (long)archive_offset_, SEEK_SET);
    if (fread(&header_, sizeof(mpq_header_v1), 1, file_) != 1) {
        return false;
    }

    if (memcmp(header_.magic, "MPQ\x1A", 4) != 0) {
        return false;
    }

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

    fseek(file_, (long)(archive_offset_ + block.file_offset), SEEK_SET);

    std::vector<uint8_t> raw(block.compressed_size);
    if (fread(raw.data(), 1, block.compressed_size, file_) != block.compressed_size) {
        return {};
    }

    if (block.flags & MPQ_FILE_ENCRYPTED) {
        const char* base = strrchr(filename, '\\');
        if (!base) {
            base = strrchr(filename, '/');
        }
        const char* key_name = base ? base + 1 : filename;

        uint32_t file_key = HashString(key_name, HASH_TYPE_FILE_KEY, crypt_table_);

        if (block.flags & MPQ_FILE_FIX_KEY) {
            file_key = (file_key + block.file_offset) ^ block.uncompressed_size;
        }

        DecryptBlock(reinterpret_cast<uint32_t*>(raw.data()),
                     block.compressed_size / sizeof(uint32_t),
                     file_key, crypt_table_);
    }

    if (block.flags & MPQ_FILE_COMPRESS) {
        if (block.compressed_size < block.uncompressed_size) {
            uint8_t comp_type = raw[0];
            std::vector<uint8_t> output(block.uncompressed_size);
            if (!Decompress(comp_type, raw.data() + 1,
                            block.compressed_size - 1,
                            output.data(), block.uncompressed_size)) {
                return {};
            }
            return output;
        }
    }

    if (block.flags & MPQ_FILE_IMPLODE) {
        return {};
    }

    return raw;
}
