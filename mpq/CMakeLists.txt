add_library(mollen-storm-mpq STATIC
    crypto.cpp
    hash_table.cpp
    mpq_archive.cpp
)

target_include_directories(mollen-storm-mpq PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(mollen-storm-mpq PUBLIC mollen-storm-common)
