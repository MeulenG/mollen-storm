cmake_minimum_required(VERSION 3.16)
project(mollen-storm)

set(CMAKE_CXX_STANDARD 17)

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
