#include "compression.h"

bool Decompress(uint8_t compression_type,
                const uint8_t* input, uint32_t input_size,
                uint8_t* output, uint32_t output_size) {
    switch (compression_type) {
        case COMPRESSION_ZLIB:
            return false;
        case COMPRESSION_PKWARE:
            return false;
        case COMPRESSION_BZIP2:
            return false;
        case COMPRESSION_LZMA:
            return false;
        default:
            return false;
    }
}
