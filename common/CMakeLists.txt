add_library(mollen-storm-common STATIC
    compression.cpp
)

target_include_directories(mollen-storm-common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
