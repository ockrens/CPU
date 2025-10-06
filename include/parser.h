/**
 * @file parser.h
 * @brief Declaration of the Parser class for assembly instruction parsing.
 * @version 0.2.1
 * @date 2025-10-06
 */

#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
using namespace std; // ✅ Use standard namespace

/**
 * @brief Represents a parsed assembly instruction.
 */
struct Instruction {
    string opcode;
    string operand1;
    string operand2;
};

/**
 * @brief Class responsible for parsing assembly instructions.
 */
class Parser {
public:
    /**
     * @brief Default constructor.
     */
    Parser() = default;

    /**
     * @brief Parses a single assembly instruction line into its components.
     * 
     * @param line The input assembly line (e.g., "SUBC R2, R6")
     * @return Instruction Struct containing mnemonic, operand1, and operand2.
     */
    Instruction parseInstruction(const string& line) const;
};

#endif // PARSER_H
