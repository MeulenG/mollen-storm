#ifndef CRYPTO_H
#define CRYPTO_H

#include <cstdint>

#define CRYPT_TABLE_SIZE 0x500

#define HASH_TYPE_TABLE_OFFSET 0x000
#define HASH_TYPE_NAME_A       0x100
#define HASH_TYPE_NAME_B       0x200
#define HASH_TYPE_FILE_KEY     0x300

void InitCryptTable(uint32_t* crypt_table);
uint32_t HashString(const char* key, uint32_t hash_type, const uint32_t* crypt_table);
void DecryptBlock(uint32_t* data, uint32_t dword_count, uint32_t key, const uint32_t* crypt_table);

#endif // CRYPTO_H
