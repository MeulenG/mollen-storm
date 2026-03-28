#include "casc_archive.h"
#include <cstring>

CascArchive::CascArchive() {
    memset(data_dir_, 0, sizeof(data_dir_));
}

CascArchive::~CascArchive() {
    Close();
}

bool CascArchive::Open(const char* data_dir) {
    (void)data_dir;
    return false;
}

void CascArchive::Close() {
    memset(data_dir_, 0, sizeof(data_dir_));
}

std::vector<uint8_t> CascArchive::ExtractFile(const char* filename) {
    (void)filename;
    return {};
}
