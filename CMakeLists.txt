cmake_minimum_required(VERSION 3.30.1)

message(NOTICE " " CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE})
message(NOTICE " " CMAKE_CURRENT_BINARY_DIR = ${CMAKE_CURRENT_BINARY_DIR})

project(BitmapLibrary
    VERSION 1.0.2
    LANGUAGES CXX
)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra")

# The executable code is here
add_subdirectory(src)

# The executable app is here
add_subdirectory(app)

if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME) 
    AND (CMAKE_BUILD_TYPE STREQUAL Debug))
    add_subdirectory(test)        
endif()



# if(CMAKE_BUILD_TYPE STREQUAL Release)
#     # Other stuff potentially
# else()
#     # Testing only available if this is the main app
    
# endif()