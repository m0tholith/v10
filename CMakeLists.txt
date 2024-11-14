cmake_minimum_required(VERSION 3.12)

set(BUILD_DEBUG ON)
set(ENABLE_ERRORCHECKING ON)

set(SRC_FILES
    src/glad.c
    src/rendering.c
    src/window.c
    src/input.c
    src/mesh.c
    src/shader.c
    src/stb_image.c
    src/texture.c
    src/cubemap.c
    src/model.c
    src/animation.c
    src/node.c
    src/camera.c
    src/material.c
    src/main.c
    src/armature.c
    src/scene_object.c
    src/framebuffer.c
    src/light.c
    src/error.c
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(BUILD_DEBUG)
  add_compile_options("-g")
  add_compile_options("-Wall")
  add_compile_options("-Werror")
  add_compile_options("-D BUILD_DEBUG")
endif()
if(ENABLE_ERRORCHECKING)
  add_compile_options("-D ENABLE_ERRORCHECKING")
endif()

project(game)
add_executable(game ${SRC_FILES})
target_include_directories(game PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(game m GL glfw cglm assimp)
