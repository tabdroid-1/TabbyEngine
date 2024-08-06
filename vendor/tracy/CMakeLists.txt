cmake_minimum_required(VERSION 3.19)
project(tracy)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE PROJECT_SOURCES
        "TracyClient.cpp"
        "tracy/Tracy.hpp"
        )

# if(NOT EMSCRIPTEN AND NOT ANDROID)
list(APPEND PROJECT_SOURCES
        "tracy/TracyOpenGL.hpp"
        )
# endif()

add_library(${PROJECT_NAME} STATIC
        ${PROJECT_SOURCES}
        )


if(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -DDEBUG -DTRACY_ENABLE=1")
endif()
