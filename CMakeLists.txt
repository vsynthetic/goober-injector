cmake_minimum_required(VERSION 3.12)

project(goober-injector CXX)

add_compile_options(-std=c++23)

set(GOOBER_SOURCES
    src/entrypoint.cpp
    src/ipc/ipc.cpp
    src/injector/injector.cpp
    src/imgui/imgui_wrap.cpp)

set(GOOBER_HEADERS
    src/ipc/ipc.hpp
    src/injector/injector.hpp
    src/network/messages.hpp
    src/imgui/imgui_wrap.hpp)

add_executable(goober-injector
    ${GOOBER_SOURCES}
    ${GOOBER_HEADERS})

add_subdirectory(ext/imgui)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

target_link_libraries(goober-injector PRIVATE imgui glfw OpenGL::GL)

target_include_directories(goober-injector PRIVATE ext/imgui ext/imgui/backends)
