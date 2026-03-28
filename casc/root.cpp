#include "root.h"

bool RootFile::Load(const uint8_t* data, uint32_t size) {
    (void)data;
    (void)size;
    return false;
}

bool RootFile::Lookup(const char* filename, uint8_t* content_hash_out) const {
    (void)filename;
    (void)content_hash_out;
    return false;
}
