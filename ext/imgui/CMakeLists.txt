cmake_minimum_required(VERSION 3.12)

project(IMGUI CXX)

set(IMGUI_SOURCES
    imgui.cpp
    imgui_demo.cpp
    imgui_draw.cpp
    imgui_stdlib.cpp
    imgui_tables.cpp
    imgui_widgets.cpp
    backends/imgui_impl_glfw.cpp
    backends/imgui_impl_opengl3.cpp)

set(IMGUI_HEADERS
    imgui.h
    imconfig.h
    imgui_internal.h
    imgui_stdlib.h
    imstb_rectpack.h
    imstb_textedit.h
    imstb_truetype.h
    backends/imgui_impl_glfw.h
    backends/imgui_impl_opengl3.h
    backends/imgui_impl_opengl3_loader.h)

add_library(imgui STATIC
    ${IMGUI_SOURCES}
    ${IMGUI_HEADERS})
