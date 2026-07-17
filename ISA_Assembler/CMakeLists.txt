cmake_minimum_required(VERSION 3.10)

project(ISA_Toolchain VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 1. Define the shared config file (exclude it from the main files)
set(SHARED_SRC "${CMAKE_SOURCE_DIR}/src/cpu_config.cpp")

# 2. Build the Assembler Executable
add_executable(ISA_Assembler 
    "${CMAKE_SOURCE_DIR}/src/assembler_main.cpp" 
    ${SHARED_SRC}
)

# 3. Build the Engine Executable
add_executable(ISA_Instruction_Encoder
    "${CMAKE_SOURCE_DIR}/src/instruction_decoder_main.cpp" 
    ${SHARED_SRC}
)

# 4. Set Include Directories for BOTH targets
target_include_directories(ISA_Assembler PRIVATE "${CMAKE_SOURCE_DIR}/include" "C:/msys64/mingw64/include")
target_include_directories(ISA_Instruction_Encoder   PRIVATE "${CMAKE_SOURCE_DIR}/include" "C:/msys64/mingw64/include")

# 5. Set Output Directories so both .exe files land in your root folder
set_target_properties(ISA_Assembler PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set_target_properties(ISA_Instruction_Encoder    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})