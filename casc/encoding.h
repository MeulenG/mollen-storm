#ifndef ENCODING_H
#define ENCODING_H

#include <cstdint>

class EncodingFile {
public:
    bool Load(const uint8_t* data, uint32_t size);
    bool Lookup(const uint8_t* content_hash, uint8_t* encoding_key_out) const;

private:
    const uint8_t* data_;
    uint32_t size_;
};

#endif // ENCODING_H
