

# CMakeLists.txt

# Minimum CMake version
cmake_minimum_required(VERSION 3.10)

# Project name and language
project(CPU-Assembler VERSION 0.1.0 LANGUAGES CXX)

# Set the C standard (optional)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(include)

# Source files
file(GLOB SRC_FILES src/*.cpp)

# Executable target
add_executable(CPU-Assembler ${SRC_FILES})
set_target_properties(CPU-Assembler PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})