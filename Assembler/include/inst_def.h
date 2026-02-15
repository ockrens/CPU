/**
 * @file inst_def.h
 * @brief Definition of instruction formats and opcodes.
 * 
 * @author R.J. Ockhuijsen
 * @date 2026-01-28
 * @version 0.1
 * 
 * @details
 * This file contains the definitions for instruction formats, these can be edited here to change the way the are encoded.
 */

#ifndef INST_DEF_H
#define INST_DEF_H

#include <string>
#include <optional>
#include <vector>



enum class RSMode {
    R8 = 0,
    R16 = 1,
    imm4 = 3,
    imm8 = 2,
};

enum class RDMode { // do i need this?
    R = 0,
    W = 0,
    RW = 0
};

// (opcode, func3, func2, func1, func0, RS, RD)
struct OpcodeFormat {
    std::string opcode;
    std::optional<int> field3; // bits 12..15
    std::optional<int> field2; // bits 8..11
    std::optional<int> field1; // bits 4..7
    std::optional<int> field0; // bits 0..3
    std::optional<RSMode> RS;
    std::optional<RDMode> RD;




    OpcodeFormat(
        std::string opcode,
        std::optional<int> func3 = std::nullopt,
        std::optional<int> func2 = std::nullopt,
        std::optional<int> func1 = std::nullopt,
        std::optional<int> func0 = std::nullopt,
        std::optional<RSMode> RS = std::nullopt,
        std::optional<RDMode> RD = std::nullopt

    )
        : opcode(std::move(opcode)),
            RS(RS),
            RD(RD),
            field0(func0),
            field1(func1),
            field2(func2),
            field3(func3)
    {}
};

inline std::vector<OpcodeFormat> OPCODES = {
    OpcodeFormat("NOP",  8, 4, 2, 1),
    OpcodeFormat("HALT", 0,0,0,0),
    OpcodeFormat("LDI", 0,0,0,3, RSMode::imm8),
};

#endif