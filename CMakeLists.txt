cmake_minimum_required(VERSION 3.16)
project(mollen-storm)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(
    zlib
    GIT_REPOSITORY https://github.com/madler/zlib.git
    GIT_TAG        v1.3.1
)
set(ZLIB_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(zlib)

FetchContent_Declare(
    bzip2
    GIT_REPOSITORY https://github.com/libarchive/bzip2.git
    GIT_TAG        bzip2-1.0.8
    GIT_SHALLOW    TRUE
)
FetchContent_GetProperties(bzip2)
if(NOT bzip2_POPULATED)
    FetchContent_Populate(bzip2)
endif()

add_library(bzip2 STATIC
    ${bzip2_SOURCE_DIR}/blocksort.c
    ${bzip2_SOURCE_DIR}/huffman.c
    ${bzip2_SOURCE_DIR}/crctable.c
    ${bzip2_SOURCE_DIR}/randtable.c
    ${bzip2_SOURCE_DIR}/compress.c
    ${bzip2_SOURCE_DIR}/decompress.c
    ${bzip2_SOURCE_DIR}/bzlib.c
)
target_include_directories(bzip2 PUBLIC ${bzip2_SOURCE_DIR})

FetchContent_Declare(
    stormlib
    GIT_REPOSITORY https://github.com/ladislav-zezula/StormLib.git
    GIT_TAG        v9.25
    GIT_SHALLOW    TRUE
)
FetchContent_GetProperties(stormlib)
if(NOT stormlib_POPULATED)
    FetchContent_Populate(stormlib)
endif()

add_library(storm-compression STATIC
    ${stormlib_SOURCE_DIR}/src/huffman/huff.cpp
    ${stormlib_SOURCE_DIR}/src/adpcm/adpcm.cpp
    ${stormlib_SOURCE_DIR}/src/sparse/sparse.cpp
    ${stormlib_SOURCE_DIR}/src/pklib/explode.c
    ${stormlib_SOURCE_DIR}/src/pklib/implode.c
    ${stormlib_SOURCE_DIR}/src/lzma/C/LzmaDec.c
)
target_include_directories(storm-compression PUBLIC
    ${stormlib_SOURCE_DIR}/src/huffman
    ${stormlib_SOURCE_DIR}/src/adpcm
    ${stormlib_SOURCE_DIR}/src/sparse
    ${stormlib_SOURCE_DIR}/src/pklib
    ${stormlib_SOURCE_DIR}/src/lzma/C
)

option(MOLLEN_STORM_BUILD_MPQ "Build MPQ support" ON)
option(MOLLEN_STORM_BUILD_CASC "Build CASC support" OFF)

add_subdirectory(common)

if(MOLLEN_STORM_BUILD_MPQ)
    add_subdirectory(mpq)
endif()

if(MOLLEN_STORM_BUILD_CASC)
    add_subdirectory(casc)
endif()

add_executable(mollen-storm main.cpp)
target_link_libraries(mollen-storm PRIVATE mollen-storm-common)

if(MOLLEN_STORM_BUILD_MPQ)
    target_link_libraries(mollen-storm PRIVATE mollen-storm-mpq)
    target_compile_definitions(mollen-storm PRIVATE MOLLEN_STORM_HAS_MPQ)
endif()

if(MOLLEN_STORM_BUILD_CASC)
    target_link_libraries(mollen-storm PRIVATE mollen-storm-casc)
    target_compile_definitions(mollen-storm PRIVATE MOLLEN_STORM_HAS_CASC)
endif()
