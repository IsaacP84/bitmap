add_executable(
    app
    app.cpp
)

target_link_libraries(app PRIVATE bitmap)
target_compile_features(app PUBLIC cxx_std_17)

# Add a custom command to copy runtime DLLs after building the executable
if (WIN32) # Ensure this only runs on Windows where DLLs are relevant
    add_custom_command(
        TARGET app
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_RUNTIME_DLLS:app>
                $<TARGET_FILE_DIR:app>
        COMMAND_EXPAND_LISTS # Important for expanding the list of DLLs
    )
endif()