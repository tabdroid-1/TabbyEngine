cmake_minimum_required(VERSION 3.19)

project(imgui)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


add_library(imgui STATIC
        imgui.cpp
        imgui_demo.cpp
        imgui_draw.cpp
        imgui_tables.cpp
        imgui_widgets.cpp

        imgui.h
        imgui_internal.h
        imstb_rectpack.h
        imstb_textedit.h
        imstb_truetype.h
        )

if(ANDROID)
target_link_libraries(imgui GLESv3)
elseif(EMSCRIPTEN)

else()
find_package(OpenGL REQUIRED)
target_link_libraries(imgui OpenGL::GL)
endif()
