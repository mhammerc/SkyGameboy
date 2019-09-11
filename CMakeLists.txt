cmake_minimum_required(VERSION 3.14)
project(fractal)

set(CMAKE_CXX_STANDARD 17)

# SFML

add_subdirectory(${CMAKE_SOURCE_DIR}/deps/SFML)
include_directories(${CMAKE_SOURCE_DIR}/deps/SFML/include)

add_executable(fractal src/main.cpp src/cpu.cpp src/cpu.h src/file_reader.h src/general.h src/virtual_memory.cpp src/virtual_memory.h)
target_link_libraries(fractal sfml-system sfml-window sfml-graphics)