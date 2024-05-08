cmake_minimum_required(VERSION 3.12)

set(BUILD_DEBUG ON)

set(SRC_FILES
    src/main.c
    src/glad.c
    src/window.c
    src/input.c
    src/mesh.c
    src/model.c
    src/node.c
    src/shader.c
    src/material.c
    src/texture.c
    src/stb_image.c
    src/camera.c
)

if(BUILD_DEBUG)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    add_compile_options("-g")
endif()

project(game)
add_executable(game ${SRC_FILES})
target_include_directories(game PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_directories(game PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lib")

find_library(cglm "libcglm.a")
find_library(assimp "libassimp.so")
target_link_libraries(game m GL glfw cglm assimp)
