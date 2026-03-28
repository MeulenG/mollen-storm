#ifndef BLTE_H
#define BLTE_H

#include <cstdint>
#include <vector>

class BlteDecoder {
public:
    bool Decode(const uint8_t* input, uint32_t input_size,
                std::vector<uint8_t>& output);
};

#endif // BLTE_H
