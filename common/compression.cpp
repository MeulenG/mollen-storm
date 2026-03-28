#include "compression.h"
#include <zlib.h>

bool Decompress(uint8_t compression_type,
                const uint8_t* input, uint32_t input_size,
                uint8_t* output, uint32_t output_size) {
    switch (compression_type) {
        case COMPRESSION_ZLIB: {
            uLongf dest_len = output_size;
            int ret = uncompress(output, &dest_len, input, input_size);
            return ret == Z_OK;
        }
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
