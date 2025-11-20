/**
 * @file parser.h
 * @brief Declaration of the Parser class for assembly instruction parsing.
 * @version 0.2.1
 * @date 2025-10-06
 */

#ifndef PARSER_H
#define PARSER_H

#define instructionLength uint16_t
#define MemoryWidth uint8_t

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

    Instruction() : opcode(""), operand1(""), operand2("") {}

    ~Instruction(){}

    void reset(){
        opcode.clear();
        operand1.clear();
        operand2.clear();
    }
};
struct DecodedInstruction{
    string opcodeName;
    instructionLength opcode;
    instructionLength regDest;
    instructionLength regSrc;
    bool isImmediate = false;
    bool fitInImm4 = false;
    instructionLength immediate_8;
    instructionLength immediate_4;

    DecodedInstruction() : opcodeName(""), opcode(0), regDest(0), regSrc(0), isImmediate(false), immediate_8(0), immediate_4(0), fitInImm4(false){}

    ~DecodedInstruction(){}

    void reset(){
        opcodeName.clear();
        opcode = 0;
        regDest = 0;
        regSrc = 0;
        isImmediate = false;
        immediate_8 = 0;
    }
};


enum class Opcode : uint8_t {
    NOP   = 0x00,
    ADD   = 0x01,
    SUB   = 0x02,
    MOV   = 0x03,
    LOAD  = 0x04,
    STORE = 0x05,
    JMP   = 0x06,
    CMP   = 0x07,
    INVALID = 0xFF
};

enum class Register : uint8_t {
    R0   = 0x01,
    R1   = 0x02,
    R2   = 0x03,
    R3   = 0x04,
    R4   = 0x05,
    R5   = 0x06,
    R6   = 0x07,
    INVALID = 0x00
};


/**
 * @brief Class responsible for parsing assembly instructions.
 */
class Parser {
public:
    /**
     * @brief Default constructor.
     */
    Parser() {};

    /**
     * @brief Parses a single assembly instruction line into its components.
     * 
     * @param line The input assembly line (e.g., "SUBC R2, R6")
     * @return Instruction Struct containing mnemonic, operand1, and operand2.
     */
    Instruction parseInstruction(const string& line) const;

    DecodedInstruction decodeInstruction(const Instruction& inst) const;

    int encodeBinary(const DecodedInstruction& decoded) const;

};

#endif // PARSER_H
