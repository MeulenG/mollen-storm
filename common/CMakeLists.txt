add_library(mollen-storm-common STATIC
    compression.cpp
)

target_include_directories(mollen-storm-common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(mollen-storm-common PUBLIC zlibstatic bzip2 storm-compression)
target_include_directories(mollen-storm-common PUBLIC ${zlib_SOURCE_DIR} ${zlib_BINARY_DIR})
