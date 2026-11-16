cmake_minimum_required(VERSION 3.10)
project(ISA_Toolchain VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 1. Gebruik OBJECT in plaats van STATIC (geen ranlib.exe nodig!)
add_library(cpu_config OBJECT "${CMAKE_SOURCE_DIR}/src/cpu_config.cpp")
target_include_directories(cpu_config PUBLIC "${CMAKE_SOURCE_DIR}/include" "C:/msys64/mingw64/include")

# 2. Build de Assembler Executable
add_executable(ISA_Assembler "${CMAKE_SOURCE_DIR}/src/assembler_main.cpp")
target_link_libraries(ISA_Assembler PRIVATE cpu_config)

# 3. Build de Instruction Decoder Executable
add_executable(ISA_Instruction_Encoder "${CMAKE_SOURCE_DIR}/src/instruction_decoder_main.cpp")
target_link_libraries(ISA_Instruction_Encoder PRIVATE cpu_config)

# 4. Zet beide .exe bestanden in je root folder
set_target_properties(ISA_Assembler ISA_Instruction_Encoder PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
)