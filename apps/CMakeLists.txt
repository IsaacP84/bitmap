add_executable(app
    app.cpp
)




# target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
# message("Magic_Engine_SOURCE_DIR: ${Magic_Engine_SOURCE_DIR}")

# target_include_directories(app PUBLIC
#     "$<BUILD_INTERFACE:${}>/include"
#     "$<INSTALL_INTERFACE:include>" # <prefix>/include/mylib
# )



target_link_libraries(app PRIVATE bitmap)




# install(TARGETS engine
#     EXPORT magicTargets
#     FILE_SET HEADERS
#     LIBRARY DESTINATION lib
#     ARCHIVE DESTINATION lib
#     RUNTIME DESTINATION bin
#     INCLUDES DESTINATION "include")

# install(EXPORT magicTargets
#     FILE magicTargets.cmake
#     NAMESPACE Magic::
#     DESTINATION lib/cmake/Magic!
# )

# install(DIRECTORY
#     # ../include/core
#     # ../include/components
#     # ../include/utils
#     ./include/magic-engine
#     DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
# )

# include(CMakePackageConfigHelpers)
# write_basic_package_version_file(
#     "magicConfigVersion.cmake"
#     VERSION ${magic_VERSION}
#     COMPATIBILITY AnyNewerVersion)

# install(FILES "magicConfig.cmake" "${CMAKE_CURRENT_BINARY_DIR}/magicConfigVersion.cmake"
#     DESTINATION lib/cmake/Magic!)

# install(FILES

#     DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
# )


# this is needed to stop the console window
# target_link_options(game
#     PRIVATE
#     -mwindows
# )


# target_link_libraries(Engine PRIVATE Magic_Debug)
# add_definitions(-DBUILD_DLL)




# set_target_properties(Magic::Engine PROPERTIES
#     INTERFACE_INCLUDE_DIRECTORIES "${_IMPORT_PREFIX}/include"
# )




target_compile_features(app PUBLIC cxx_std_17)

