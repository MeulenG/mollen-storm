#ifndef ROOT_H
#define ROOT_H

#include <cstdint>

class RootFile {
public:
    bool Load(const uint8_t* data, uint32_t size);
    bool Lookup(const char* filename, uint8_t* content_hash_out) const;

private:
    const uint8_t* data_;
    uint32_t size_;
};

#endif // ROOT_H
