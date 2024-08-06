cmake_minimum_required(VERSION 3.22)
project(tabby
	VERSION 0.0.1
    DESCRIPTION "Game engine"
	HOMEPAGE_URL "https://github.com/tabdroid-1/TabbyEngine"
    LANGUAGES CXX
)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(APPLE)
    message(STATUS "Including Objective-C++ for APPLE")
    list(APPEND LANGUAGES OBJC)
    list(APPEND LANGUAGES OBJCXX)
    set(CMAKE_OBJCXX_STANDARD 14)
    set(CMAKE_OBJCXX_STANDARD_REQUIRED ON)
    set(CMAKE_OBJCXX_EXTENSIONS OFF)
endif()


option(TB_DIST "Enable distrubition build" OFF)

if (NOT TB_DIST AND NOT WIN32)
    add_definitions(-w) # disables warnings
endif()

set(ALSOFT_UTILS OFF CACHE BOOL "Build utility programs" FORCE)
set(ALSOFT_NO_CONFIG_UTIL OFF CACHE BOOL "Disable building the alsoft-config utility" FORCE)
set(ALSOFT_EXAMPLES OFF CACHE BOOL "Build example programs" FORCE)
set(ALSOFT_TESTS OFF CACHE BOOL "Build test programs" FORCE)
set(ALSOFT_INSTALL  OFF CACHE BOOL "Install main library" FORCE)
set(ALSOFT_INSTALL_CONFIG CACHE BOOL "Install alsoft.conf sample configuration file" FORCE)
set(ALSOFT_INSTALL_HRTF_DATA CACHE BOOL "Install HRTF data files" FORCE)
set(ALSOFT_INSTALL_AMBDEC_PRESETS CACHE BOOL "Install AmbDec preset files" FORCE)
set(ALSOFT_INSTALL_EXAMPLES BOOL "Install example programs (alplay, alstream, ...)" FORCE)
# set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
set(BUILD_PROGRAMS OFF CACHE BOOL "Build programs (libsndfile)" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "Build examples (libsndfile)" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "Build tests (libsndfile)" FORCE)
set(SDL_SHARED OFF CACHE BOOL "Build a shared version of the library" FORCE)
set(SDL_STATIC ON CACHE BOOL "Build a static version of the library" FORCE)
set(SDL_TEST OFF CACHE BOOL "Build the SDL2_test library" FORCE)
set(BOX2D_SAMPLES OFF CACHE BOOL "" FORCE)
set(BOX2D_DOCS OFF CACHE BOOL "" FORCE)
set(BOX2D_PROFILE OFF CACHE BOOL "" FORCE)
set(BOX2D_BUILD_UNIT_TESTS OFF CACHE BOOL "Build the Box2D unit tests" FORCE)
set(BOX2D_BUILD_TESTBED OFF CACHE BOOL "Build the Box2D testbed" FORCE)
set(MSDF_ATLAS_BUILD_STANDALONE OFF CACHE BOOL "Disable building MSDF Standalone executable" FORCE)
set(MSDF_ATLAS_USE_VCPKG OFF CACHE BOOL "Disable MSDF VCPKG Fetch" FORCE)
set(MSDF_ATLAS_USE_SKIA OFF CACHE BOOL "Disable Skia from MSDF" FORCE)
set(MSDFGEN_DISABLE_SVG ON CACHE BOOL "Disable MSDF VCPKG Fetch" FORCE)
set(MSDFGEN_DISABLE_PNG ON CACHE BOOL "Disable MSDF VCPKG Fetch" FORCE)
set(MSDF_ATLAS_NO_ARTERY_FONT OFF CACHE BOOL "Disable Artery Font export and do not require its submodule" FORCE)
set(MSDF_ATLAS_MSDFGEN_EXTERNAL OFF CACHE BOOL "Do not build the msdfgen submodule but find it as an external package" FORCE)
set(MSDF_ATLAS_INSTALL OFF CACHE BOOL "Generate installation target" FORCE)
set(MSDF_ATLAS_DYNAMIC_RUNTIME OFF CACHE BOOL "Link dynamic runtime library instead of static" FORCE)
set(MSDFGEN_CORE_ONLY OFF CACHE BOOL "Only build the core library with no dependencies" FORCE)
set(MSDFGEN_BUILD_STANDALONE OFF CACHE BOOL "Build the msdfgen standalone executable" FORCE)
set(MSDFGEN_USE_VCPKG OFF CACHE BOOL "Use vcpkg package manager to link project dependencies" FORCE)
set(MSDFGEN_USE_OPENMP OFF CACHE BOOL "Build with OpenMP support for multithreaded code" FORCE)
set(MSDFGEN_USE_CPP11 ON CACHE BOOL "Build with C++11 enabled" FORCE)
set(MSDFGEN_USE_SKIA ON CACHE BOOL "Build with the Skia library" FORCE)
set(MSDFGEN_INSTALL  OFF CACHE BOOL "Generate installation target" FORCE)
set(MSDFGEN_DYNAMIC_RUNTIME OFF CACHE BOOL "Link dynamic runtime library instead of static" FORCE)
set(SPDLOG_HEADER_ONLY OFF CACHE BOOL "" FORCE)

if(TB_DIST)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build utility programs" FORCE)
endif(TB_DIST)

set(PROJECT_SOURCE_DIR "src")
set(PROJECT_VENDOR_DIR "vendor")

# ##############################################################################
# Add dependencies 
# ##############################################################################

set(GLM "${PROJECT_VENDOR_DIR}/glm")
set(STB "${PROJECT_VENDOR_DIR}/stb")
set(SDL2 "${PROJECT_VENDOR_DIR}/SDL2")
set(GLAD "${PROJECT_VENDOR_DIR}/glad")
set(ENTT "${PROJECT_VENDOR_DIR}/entt")
set(BOX2C "${PROJECT_VENDOR_DIR}/box2c")
set(BOX2D "${PROJECT_VENDOR_DIR}/box2d")
set(TRACY "${PROJECT_VENDOR_DIR}/tracy")
set(IMGUI "${PROJECT_VENDOR_DIR}/imgui")
set(SPDLOG "${PROJECT_VENDOR_DIR}/spdlog")
set(FASTGLTF "${PROJECT_VENDOR_DIR}/fastgltf")
set(IMGUIZMO "${PROJECT_VENDOR_DIR}/imguizmo")
set(OPENAL "${PROJECT_VENDOR_DIR}/openal-soft")
set(LIBSNDFILE "${PROJECT_VENDOR_DIR}/libsndfile")
set(MSDF-ATLAS-GEN "${PROJECT_VENDOR_DIR}/msdf-atlas-gen")
set(TINY-OBJ-LOADER "${PROJECT_VENDOR_DIR}/tinyobjloader")

# ##############################################################################
# Project executable setup
# ##############################################################################

file(GLOB_RECURSE PROJECT_SOURCES 
        "${PROJECT_SOURCE_DIR}/*.h"
        "${PROJECT_SOURCE_DIR}/*.cpp"
        )

# # Additional project sources (vendors, etc.)
list(APPEND PROJECT_SOURCES
        "${ENTT}/entt.hpp"
        "${STB}/stb_image.h"
        "${STB}/stb_image.cpp"
        "${TINY-OBJ-LOADER}/tinyobjloader.cpp"
        )


add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}) 
target_include_directories(${PROJECT_NAME} PUBLIC ${LIBSNDFILE}/include) 
target_include_directories(${PROJECT_NAME} PUBLIC ${SDL2_INCLUDE_DIRS}) 
target_include_directories(${PROJECT_NAME} PUBLIC ${FASTGLTF}/include) 
target_include_directories(${PROJECT_NAME} PUBLIC ${SPDLOG}/include) 
target_include_directories(${PROJECT_NAME} PUBLIC ${OPENAL}/include) 
target_include_directories(${PROJECT_NAME} PUBLIC ${BOX2C}/include) 
target_include_directories(${PROJECT_NAME} PUBLIC ${IMGUIZMO}) 
target_include_directories(${PROJECT_NAME} PUBLIC ${IMGUI}) 
target_include_directories(${PROJECT_NAME} PUBLIC ${TRACY}) 
target_include_directories(${PROJECT_NAME} PUBLIC ${ENTT}) 
target_include_directories(${PROJECT_NAME} PUBLIC ${STB}) 
target_include_directories(${PROJECT_NAME} PUBLIC ${GLM}) 
target_include_directories(${PROJECT_NAME} PRIVATE ${MSDF-ATLAS-GEN}/msdf-atlas-gen) 
target_include_directories(${PROJECT_NAME} PRIVATE ${TINY-OBJ-LOADER}) 
target_include_directories(${PROJECT_NAME} PRIVATE ${GLAD}) 
target_include_directories(${PROJECT_NAME} PRIVATE ${STB}) 

# ####################
# Add Subdirectories
# ####################

add_subdirectory(${GLM})
add_subdirectory(${SDL2})
add_subdirectory(${IMGUI})
add_subdirectory(${BOX2C})
add_subdirectory(${TRACY})
add_subdirectory(${SPDLOG})
add_subdirectory(${IMGUIZMO})
add_subdirectory(${FASTGLTF})
add_subdirectory(${LIBSNDFILE})
add_subdirectory(${MSDF-ATLAS-GEN})


if(NOT EMSCRIPTEN)
    add_subdirectory(${OPENAL})
endif()

if (WIN32)

    if (MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE /experimental:c11atomics console)


        if(CMAKE_BUILD_TYPE MATCHES "Debug")
            set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI")
            set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /SUBSYSTEM:CONSOLE")
            set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
        endif()
    endif()

    if (MINGW)
        set(CMAKE_SYSTEM_NAME Windows)
        set(ENABLE_STATIC_RUNTIME ON CACHE BOOL "Enable static runtime" FORCE)
        target_link_options(${PROJECT_NAME} PUBLIC -static)

        # does this even work?
        if(MAINECOON_DIST)
            target_link_options(${PROJECT_NAME} PUBLIC -mwindows)
        else()
            target_link_options(${PROJECT_NAME} PUBLIC -mconsole)
        endif()
    endif()

    message(STATUS "Generating scripts for Windows")
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        glm
        imgui
        box2d
        tracy 
        sndfile
        fastgltf
        imguizmo
        OpenAL::OpenAL
        msdf-atlas-gen 
        SDL2::SDL2main
        # spdlog::spdlog
        SDL2::SDL2-static
        )
elseif (UNIX AND NOT APPLE AND NOT ANDROID AND NOT EMSCRIPTEN)

    find_package(OpenAL)
    target_link_options(${PROJECT_NAME} PUBLIC -ludev)

    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)

    message(STATUS "Generating scripts for Linux")
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        glm
        tracy 
        imgui
        box2d
        spdlog
        sndfile
        imguizmo
        fastgltf
        OpenAL::OpenAL
        msdf-atlas-gen 
        PkgConfig::GTK4
        SDL2::SDL2-static
        )
elseif (APPLE)
    message(STATUS "Generating scripts for Mac")
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        glm
        tracy 
        imgui
        box2d
        spdlog
        fastgltf
        imguizmo
        msdf-atlas-gen 
        SDL2::SDL2-static
        )
elseif (EMSCRIPTEN)
    add_compile_definitions("__EMSCRIPTEN__")
    # target_compile_options(${PROJECT_NAME} PUBLIC -sGL_PREINITIALIZED_CONTEXT=1)
    # target_compile_options(${PROJECT_NAME} PUBLIC -sOFFSCREEN_FRAMEBUFFER=1)
    # target_compile_options(${PROJECT_NAME} PUBLIC -sMIN_WEBGL_VERSION=2)
    # target_compile_options(${PROJECT_NAME} PUBLIC -sMAX_WEBGL_VERSION=2)
    # target_compile_options(${PROJECT_NAME} PUBLIC -sEXIT_RUNTIME=1)
    # target_compile_options(${PROJECT_NAME} PUBLIC -sMODULARIZE=1)
    # target_compile_options(${PROJECT_NAME} PUBLIC -sUSE_SDL=2)
    # target_link_options(${PROJECT_NAME} PUBLIC -sUSE_SDL=2)
    # target_compile_options(${PROJECT_NAME} PUBLIC -sFORCE_FILESYSTEM=1)
    # target_compile_options(${PROJECT_NAME} PUBLIC -sFETCH=1)
    # target_compile_options(${PROJECT_NAME} PUBLIC -sASYNCIFY)
    # target_compile_options(${PROJECT_NAME} PUBLIC -sASSERTIONS=1)
    # target_compile_options(${PROJECT_NAME} PUBLIC -sINVOKE_RUN=0)
    # target_compile_options(${PROJECT_NAME} PUBLIC -O2)
    # target_compile_options(${PROJECT_NAME} PUBLIC -FULL_ES3=1)
    # target_compile_options(${PROJECT_NAME} PUBLIC -sMIN_FIREFOX_VERSION=79)
    # target_compile_options(${PROJECT_NAME} PUBLIC -sMIN_SAFARI_VERSION=150000)
    # target_compile_options(${PROJECT_NAME} PUBLIC -sMIN_IE_VERSION=0x7FFFFFFF)
    # target_compile_options(${PROJECT_NAME} PUBLIC -sMIN_CHROME_VERSION=80)
    # target_compile_options(${PROJECT_NAME} PUBLIC -lGL)
    # target_compile_options(${PROJECT_NAME} PUBLIC -lopenal)
    # target_compile_options(${PROJECT_NAME} PUBLIC -lSDL2)

    target_link_options(${PROJECT_NAME} PUBLIC -pthread)
    target_compile_options(${PROJECT_NAME} PUBLIC -pthread)

    message(STATUS "Generating scripts for EMSCRIPTEN")
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        glm
        imgui
        box2d
        tracy 
        spdlog
        sndfile
        imguizmo
        fastgltf
        msdf-atlas-gen 
        # SDL2::SDL2main
        SDL2::SDL2-static
        )
elseif (ANDROID)
    message(STATUS "Generating scripts for Android")
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        log
        glm
        EGL
        imgui
        box2d
        spdlog
        GLESv2
        GLESv3
        android
        imguizmo
        fastgltf
        SDL2::SDL2
        SDL2::SDL2main
        )
endif()

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -DDEBUG -DTB_DEBUG=1")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

# Ensure that hot-reload is enabled for VS
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
endif()

target_precompile_headers(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/tbpch.h")
