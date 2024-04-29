cmake_minimum_required(VERSION 3.12)
file(GLOB SRC_FILES src/*)

if(CMAKE_BUILD_TYPE EQUAL DEBUG)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    add_compile_options("-g")
endif()

project(game)
add_executable(game ${SRC_FILES})
target_include_directories(game PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_directories(game PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lib")

target_link_libraries(game "m")
target_link_libraries(game "GL")
target_link_libraries(game "glfw")
find_library(cglm "libcglm.a")
target_link_libraries(game cglm)
target_link_libraries(game "assimp")
