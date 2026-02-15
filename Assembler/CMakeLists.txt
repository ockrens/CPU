cmake_minimum_required(VERSION 3.10)

project(CPU-Assembler VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB SRC_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_executable(CPU-Assembler ${SRC_FILES})

target_include_directories(CPU-Assembler
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

set_target_properties(CPU-Assembler PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
)