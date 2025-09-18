add_library(
    bitmap
    SHARED
    bitmap.cpp
    color.cpp
)

target_compile_definitions(bitmap PRIVATE BUILD_DLL)
target_compile_features(bitmap PUBLIC cxx_std_17)

target_include_directories(bitmap PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
