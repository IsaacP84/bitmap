add_executable(
    bitmap_app
    app.cpp
)
set_target_properties(bitmap_app PROPERTIES
  OUTPUT_NAME "app"
)

target_link_libraries(bitmap_app PRIVATE bitmap)
target_compile_features(bitmap_app PRIVATE cxx_std_17)

# Add a custom command to copy runtime DLLs after building the executable
if (WIN32) # Ensure this only runs on Windows where DLLs are relevant
    add_custom_command(
        TARGET bitmap_app
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_RUNTIME_DLLS:bitmap_app>
                $<TARGET_FILE_DIR:bitmap_app>
        COMMAND_EXPAND_LISTS # Important for expanding the list of DLLs
    )
endif()