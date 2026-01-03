cmake_minimum_required(VERSION 3.10)
project(godotcoacd)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/CoACD-godot-src)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/godot-cpp)

file(GLOB_RECURSE GODOT_COACD_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library(godotcoacd SHARED ${GODOT_COACD_SRC})

target_link_libraries(godotcoacd godot-cpp coacd spdlog::spdlog)