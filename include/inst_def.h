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


enum class IMMMode {
    imm4 = 1,
    imm8 = 2,
    imm16 = 3
};
enum class RSMode {
    imm4 = 1,
    imm8 = 2,
    imm16 = 3
};
// (opcode, func0, func1, func2, func3, IMMMode, RSMode)
struct OpcodeFormat {
    std::string opcode;
    std::optional<int> func0; // bits 0..3
    std::optional<int> func1; // bits 12..15
    std::optional<int> func2; // bits 4..7
    std::optional<int> func3; // bits 8..11
    std::optional<IMMMode> IMM;
    std::optional<RSMode> RS;

    // Constructor matching Python usage
    OpcodeFormat(
        std::string opcode,
        std::optional<int> func0 = std::nullopt,
        std::optional<int> func1 = std::nullopt,
        std::optional<int> func2 = std::nullopt,
        std::optional<int> func3 = std::nullopt,
        std::optional<IMMMode> IMM = std::nullopt,
        std::optional<RSMode> RS = std::nullopt
    )
        : opcode(std::move(opcode)),
          func0(func0),
          func1(func1),
          func2(func2),
          func3(func3),
          IMM(IMM),
          RS(RS)
    {}
};

inline std::vector<OpcodeFormat> OPCODES = {
    OpcodeFormat("nop",  0, 0, 0, 0),
    OpcodeFormat("halt", 9, 0, 0)
};

#endif