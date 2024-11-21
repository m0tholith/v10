cmake_minimum_required(VERSION 3.12)

set(BUILD_DEBUG ON)
set(ENABLE_ERRORCHECKING ON)

set(SRC_FILES
    src/v10/glad.c
    src/v10/rendering.c
    src/v10/window.c
    src/v10/input.c
    src/v10/mesh.c
    src/v10/shader.c
    src/v10/stb_image.c
    src/v10/texture.c
    src/v10/cubemap.c
    src/v10/skybox.c
    src/v10/model.c
    src/v10/animation.c
    src/v10/node.c
    src/v10/camera.c
    src/v10/material.c
    src/v10/armature.c
    src/v10/entity.c
    src/v10/framebuffer.c
    src/v10/light.c
    src/v10/error.c)

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
