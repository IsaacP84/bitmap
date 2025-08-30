add_library(
    bitmap
    SHARED
    bitmap.cpp
)

target_compile_features(bitmap PUBLIC cxx_std_17)

target_include_directories(bitmap PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
add_definitions(-DBUILD_DLL)