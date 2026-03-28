#include <cstdio>
#include <vector>
#include <string>

#ifdef MOLLEN_STORM_HAS_MPQ
#include "mpq_archive.h"
#endif

#ifdef MOLLEN_STORM_HAS_CASC
#include "casc_archive.h"
#endif

int main(int argc, char* argv[]) {
    const char* path = "lichking.MPQ";
    if (argc > 1) {
        path = argv[1];
    }

#ifdef MOLLEN_STORM_HAS_MPQ
    MpqArchive mpq;
    if (mpq.Open(path)) {
        printf("Opened MPQ: %s\n", path);

        std::vector<std::string> files = mpq.GetListFile();
        if (files.empty()) {
            printf("No listfile found or archive is empty.\n");
        } else {
            printf("Files in archive (%zu):\n", files.size());
            for (const std::string& name : files) {
                printf("  %s\n", name.c_str());
            }
        }

        mpq.Close();
    }
#endif

#ifdef MOLLEN_STORM_HAS_CASC
    CascArchive casc;
    if (casc.Open(path)) {
        printf("Opened CASC: %s\n", path);
        casc.Close();
    }
#endif

    return 0;
}
