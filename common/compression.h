#ifndef COMPRESSION_H
#define COMPRESSION_H

#include <cstdint>
#include <vector>

#define COMPRESSION_HUFFMAN      0x01
#define COMPRESSION_ZLIB         0x02
#define COMPRESSION_PKWARE       0x08
#define COMPRESSION_BZIP2        0x10
#define COMPRESSION_SPARSE       0x20
#define COMPRESSION_ADPCM_MONO   0x40
#define COMPRESSION_ADPCM_STEREO 0x80
#define COMPRESSION_LZMA         0x12

bool Decompress(uint8_t compression_mask,
                const uint8_t* input, uint32_t input_size,
                uint8_t* output, uint32_t* output_size);

#endif // COMPRESSION_H
