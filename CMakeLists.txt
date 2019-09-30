cmake_minimum_required(VERSION 3.14)
project(fractal)

set(CMAKE_CXX_STANDARD 17)

# SFML
add_subdirectory(${CMAKE_SOURCE_DIR}/deps/SFML)
include_directories(${CMAKE_SOURCE_DIR}/deps/SFML/include)

# dbg-macro
add_subdirectory(${CMAKE_SOURCE_DIR}/deps/dbg-macro)
include_directories(${CMAKE_SOURCE_DIR}/deps/dbg-macro)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DDBG_MACRO_DISABLE -DDBG_MACRO_NO_WARNING")

add_executable(fractal src/main.cpp src/backend/cpu/cpu.cpp src/backend/cpu/cpu.h src/files/file_reader_stack.h src/general.h src/backend/virtual_memory.cpp src/backend/virtual_memory.h src/backend/cpu/cpu_decode.cpp src/backend/cpu/cpu_execute.cpp src/files/file_reader_heap.h src/backend/lcd.cpp src/backend/lcd.h src/frontend/display.cpp src/frontend/display.h src/backend/motherboard.h src/frontend/interfaces/i_display.h src/frontend/interfaces/i_input.h src/backend/input_manager.h)
target_link_libraries(fractal sfml-system sfml-window sfml-graphics)

target_compile_options(fractal PRIVATE -Wall -Wextra)