# Emergency override MODERN_CMAKE_BUILD_TESTING provided as well
   

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        release-1.12.1 # Or a specific commit hash
)


FetchContent_MakeAvailable(googletest)

add_executable(
    TestInitiator
    testBitDepth_24.cpp
)

target_compile_features(TestInitiator PUBLIC cxx_std_17)
target_include_directories(TestInitiator PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# target_include_directories(test PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../include")

target_link_libraries(TestInitiator PRIVATE bitmap)
target_link_libraries(TestInitiator PRIVATE GTest::gtest GTest::gtest_main)

include(GoogleTest)

enable_testing()

gtest_discover_tests(TestInitiator)



# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)


add_test(
    NAME RUN
    COMMAND TestInitiator
)

# Add a custom command to copy runtime DLLs after building the executable
if (WIN32) # Ensure this only runs on Windows where DLLs are relevant
    add_custom_command(
        TARGET TestInitiator
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_RUNTIME_DLLS:TestInitiator>
                $<TARGET_FILE_DIR:TestInitiator>
        COMMAND_EXPAND_LISTS # Important for expanding the list of DLLs
    )
endif()