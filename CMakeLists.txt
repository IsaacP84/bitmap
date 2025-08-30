cmake_minimum_required(VERSION 3.30.1)
project(bitmap
    VERSION 1.0.1
    LANGUAGES CXX
)

# The compiled library code is here
add_subdirectory(src)

# The executable code is here
# add_subdirectory(apps)
# install(EXPORT bitmapTargets DESTINATION lib/bitmap)

# Testing only available if this is the main app
# Emergency override MODERN_CMAKE_BUILD_TESTING provided as well
if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR BUILD_TESTING)
    AND BUILD_TESTING)
    add_subdirectory(test)
endif()


