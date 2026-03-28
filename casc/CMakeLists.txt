add_library(mollen-storm-casc STATIC
    idx_file.cpp
    blte.cpp
    encoding.cpp
    root.cpp
    casc_archive.cpp
)

target_include_directories(mollen-storm-casc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(mollen-storm-casc PUBLIC mollen-storm-common)
