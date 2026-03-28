#include "compression.h"
#include <zlib.h>
#include <bzlib.h>
#include <cstring>
#include <cstdlib>
#include <vector>

extern "C" {
#include "pklib.h"
#include "LzmaDec.h"
}

#include "huff.h"
#include "adpcm.h"
#include "sparse.h"

static bool DecompressHuffman(const uint8_t* input, uint32_t input_size,
                              uint8_t* output, uint32_t* output_size) {
    THuffmannTree ht(false);
    TInputStream is((void*)input, (size_t)input_size);
    unsigned int result = ht.Decompress(output, *output_size, &is);
    if (result > 0) {
        *output_size = result;
        return true;
    }
    return false;
}

static bool DecompressZlib(const uint8_t* input, uint32_t input_size,
                           uint8_t* output, uint32_t* output_size) {
    uLongf dest_len = *output_size;
    int ret = uncompress(output, &dest_len, input, input_size);
    if (ret == Z_OK) {
        *output_size = (uint32_t)dest_len;
        return true;
    }
    return false;
}

struct PkContext {
    const uint8_t* in_data;
    uint32_t in_size;
    uint32_t in_pos;
    uint8_t* out_data;
    uint32_t out_size;
    uint32_t out_pos;
};

static unsigned int pk_read(char* buf, unsigned int* size, void* param) {
    PkContext* ctx = (PkContext*)param;
    uint32_t avail = ctx->in_size - ctx->in_pos;
    uint32_t to_read = (*size < avail) ? *size : avail;
    memcpy(buf, ctx->in_data + ctx->in_pos, to_read);
    ctx->in_pos += to_read;
    return to_read;
}

static void pk_write(char* buf, unsigned int* size, void* param) {
    PkContext* ctx = (PkContext*)param;
    uint32_t avail = ctx->out_size - ctx->out_pos;
    uint32_t to_write = (*size < avail) ? *size : avail;
    memcpy(ctx->out_data + ctx->out_pos, buf, to_write);
    ctx->out_pos += to_write;
}

static bool DecompressPkware(const uint8_t* input, uint32_t input_size,
                             uint8_t* output, uint32_t* output_size) {
    PkContext ctx;
    ctx.in_data = input;
    ctx.in_size = input_size;
    ctx.in_pos = 0;
    ctx.out_data = output;
    ctx.out_size = *output_size;
    ctx.out_pos = 0;

    std::vector<char> work_buf(EXP_BUFFER_SIZE);
    unsigned int result = explode(pk_read, pk_write, work_buf.data(), &ctx);

    *output_size = ctx.out_pos;
    return (result == CMP_NO_ERROR);
}

static bool DecompressBzip2(const uint8_t* input, uint32_t input_size,
                            uint8_t* output, uint32_t* output_size) {
    unsigned int dest_len = *output_size;
    int ret = BZ2_bzBuffToBuffDecompress((char*)output, &dest_len,
                                         (char*)input, input_size, 0, 0);
    if (ret == BZ_OK) {
        *output_size = dest_len;
        return true;
    }
    return false;
}

static void* LzmaAlloc(void*, size_t size) { return malloc(size); }
static void LzmaFree(void*, void* addr) { free(addr); }
static ISzAlloc lzma_alloc = { LzmaAlloc, LzmaFree };

static bool DecompressLzma(const uint8_t* input, uint32_t input_size,
                           uint8_t* output, uint32_t* output_size) {
    const unsigned int header_size = 1 + LZMA_PROPS_SIZE;
    if (input_size <= header_size) {
        return false;
    }

    ELzmaStatus status;
    SizeT dest_len = *output_size;
    SizeT src_len = input_size - header_size;

    SRes res = LzmaDecode(output, &dest_len,
                          input + header_size, &src_len,
                          input, LZMA_PROPS_SIZE,
                          LZMA_FINISH_END, &status, &lzma_alloc);
    if (res == SZ_OK) {
        *output_size = (uint32_t)dest_len;
        return true;
    }
    return false;
}

static bool SparseDecompress(const uint8_t* input, uint32_t input_size,
                             uint8_t* output, uint32_t* output_size) {
    int out_length = (int)*output_size;
    int result = DecompressSparse(output, &out_length, (void*)input, (int)input_size);
    if (result != 0) {
        *output_size = (uint32_t)out_length;
        return true;
    }
    return false;
}

static bool AdpcmMonoDecompress(const uint8_t* input, uint32_t input_size,
                                uint8_t* output, uint32_t* output_size) {
    int out_length = DecompressADPCM(output, (int)*output_size,
                                     (void*)input, (int)input_size, 1);
    if (out_length > 0) {
        *output_size = (uint32_t)out_length;
        return true;
    }
    return false;
}

static bool AdpcmStereoDecompress(const uint8_t* input, uint32_t input_size,
                                  uint8_t* output, uint32_t* output_size) {
    int out_length = DecompressADPCM(output, (int)*output_size,
                                     (void*)input, (int)input_size, 2);
    if (out_length > 0) {
        *output_size = (uint32_t)out_length;
        return true;
    }
    return false;
}

struct DecompressEntry {
    uint8_t mask;
    bool (*func)(const uint8_t*, uint32_t, uint8_t*, uint32_t*);
};

static const DecompressEntry decompress_table[] = {
    { COMPRESSION_HUFFMAN,      DecompressHuffman     },
    { COMPRESSION_ZLIB,         DecompressZlib        },
    { COMPRESSION_PKWARE,       DecompressPkware      },
    { COMPRESSION_BZIP2,        DecompressBzip2       },
    { COMPRESSION_SPARSE,       SparseDecompress      },
    { COMPRESSION_ADPCM_MONO,   AdpcmMonoDecompress   },
    { COMPRESSION_ADPCM_STEREO, AdpcmStereoDecompress },
};

static const int decompress_table_size = sizeof(decompress_table) / sizeof(decompress_table[0]);

bool Decompress(uint8_t compression_mask,
                const uint8_t* input, uint32_t input_size,
                uint8_t* output, uint32_t* output_size) {
    if (compression_mask == COMPRESSION_LZMA) {
        return DecompressLzma(input, input_size, output, output_size);
    }

    int method_count = 0;
    for (int i = 0; i < decompress_table_size; i++) {
        if (compression_mask & decompress_table[i].mask) {
            method_count++;
        }
    }

    if (method_count == 0) {
        return false;
    }

    if (method_count == 1) {
        for (int i = 0; i < decompress_table_size; i++) {
            if (compression_mask & decompress_table[i].mask) {
                if (!decompress_table[i].func) {
                    return false;
                }
                return decompress_table[i].func(input, input_size, output, output_size);
            }
        }
        return false;
    }

    std::vector<uint8_t> temp_in(input, input + input_size);
    std::vector<uint8_t> temp_out(*output_size);
    uint32_t current_size = input_size;
    int applied = 0;

    for (int i = 0; i < decompress_table_size; i++) {
        if (!(compression_mask & decompress_table[i].mask)) {
            continue;
        }
        if (!decompress_table[i].func) {
            return false;
        }

        applied++;
        bool is_last = (applied == method_count);

        uint8_t* dst = is_last ? output : temp_out.data();
        uint32_t dst_size = *output_size;

        if (!decompress_table[i].func(temp_in.data(), current_size, dst, &dst_size)) {
            return false;
        }

        if (!is_last) {
            temp_in.assign(dst, dst + dst_size);
            current_size = dst_size;
        } else {
            *output_size = dst_size;
        }
    }

    return true;
}
