cmake_minimum_required(VERSION 3.19)

project(imguizmo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(HEADERS
        GraphEditor.h
        ImCurveEdit.h
        ImGradient.h
        ImGuizmo.h
        ImSequencer.h
        ImZoomSlider.h
    )

set(SOURCE
    GraphEditor.cpp
    ImCurveEdit.cpp
    ImGradient.cpp
    ImGuizmo.cpp
    ImSequencer.cpp

    )

add_library(imguizmo STATIC
            ${HEADERS}
            ${SOURCE}
            )

target_include_directories(${PROJECT_NAME} PRIVATE ../imgui)
