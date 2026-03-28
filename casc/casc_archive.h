#ifndef CASC_ARCHIVE_H
#define CASC_ARCHIVE_H

#include <cstdint>
#include <vector>

#include "idx_file.h"
#include "blte.h"
#include "encoding.h"
#include "root.h"

class CascArchive {
public:
    CascArchive();
    ~CascArchive();

    bool Open(const char* data_dir);
    void Close();
    std::vector<uint8_t> ExtractFile(const char* filename);

private:
    IdxFile idx_files_[16];
    BlteDecoder blte_;
    EncodingFile encoding_;
    RootFile root_;
    char data_dir_[512];
};

#endif // CASC_ARCHIVE_H
