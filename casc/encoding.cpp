#include "encoding.h"

bool EncodingFile::Load(const uint8_t* data, uint32_t size) {
    (void)data;
    (void)size;
    return false;
}

bool EncodingFile::Lookup(const uint8_t* content_hash, uint8_t* encoding_key_out) const {
    (void)content_hash;
    (void)encoding_key_out;
    return false;
}
