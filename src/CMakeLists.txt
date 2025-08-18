add_library(bitmap
    SHARED
    bitmap.cpp
)

target_compile_features(bitmap PUBLIC cxx_std_17)


# target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
# message("Magic_Engine_SOURCE_DIR: ${Magic_Engine_SOURCE_DIR}")

target_include_directories(bitmap PUBLIC
    "$<BUILD_INTERFACE:${Bitmap_SOURCE_DIR}>/include"
    "$<INSTALL_INTERFACE:include>" # <prefix>/include/mylib
)


# if(WIN32)
#     set(CMAKE_SHARED_LIBRARY_PREFIX "")
# endif()
# target_link_libraries(Magic_Debug PRIVATE engine)



install(TARGETS bitmap
    EXPORT bitmapTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)
install(FILES bitmap.h DESTINATION include)


install(EXPORT bitmapTargets
    FILE bitmapTargets.cmake
    NAMESPACE BITMAP::
    DESTINATION lib/cmake/bitmap
)

include(CMakePackageConfigHelpers)
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/bitmapConfig.cmake"
    INSTALL_DESTINATION lib/cmake/bitmap
)
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/bitmapConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/bitmapConfigVersion.cmake"
    DESTINATION lib/cmake/bitmap
)
# this is needed to stop the console window
# target_link_options(game
#     PRIVATE
#     -mwindows
# )


# target_link_libraries(Engine PRIVATE Magic_Debug)
add_definitions(-DBUILD_DLL)




# set_target_properties(Magic::Engine PROPERTIES
#     INTERFACE_INCLUDE_DIRECTORIES "${_IMPORT_PREFIX}/include"
# )





