#include "blte.h"
#include <cstring>

bool BlteDecoder::Decode(const uint8_t* input, uint32_t input_size,
                         std::vector<uint8_t>& output) {
    if (input_size < 8) {
        return false;
    }

    if (memcmp(input, "BLTE", 4) != 0) {
        return false;
    }

    (void)output;
    return false;
}
