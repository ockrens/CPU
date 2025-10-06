/**
 * @file decoder_def.h
 * @author Rens J. Ockhuijsen
 * @date 2025-02-07
 * @brief This is where the defenitions for the decoder are.
 */

#ifndef decoder_def_h
#define decoder_def_h


// Register mapping
#define R0      0x01
#define R1      0x02
#define R2      0x03
#define R3      0x04
#define R4      0x05
#define R5      0x06
#define R6      0x07


#define COUNT   0x00    // adds 2 to the pc                 does not have to have a encoding in the opcode it can be deactiveted on the JMP instructions
#define JMPR    0x01    // jump relative to current state with immediate value
#define JMP     0x02    // jump based on te lable given     need to make it so it can jump with a immediate
/**
 * x0 is immediate
 * x1 is no immediate
 * 0x is move
 * 1x is alu
 */
#define LDI     0x00    // 0000 load immediate value into register 
#define MOV     0x04    // 0100 move value from one register to another
#define ALU     0x0C    // 1100 ALU operation
#define HLT     0x0003  // 0x1111 halt the program

// alu operations
#define ADD     0x00    // 0001
#define ADDC    0x01    // 0001
#define SUB     0x02    // 0010
#define SUBC    0x03    // 0011
#define SHL     0x04    // 0100
#define SHLC    0x05    // 0101
#define SHR     0x06    // 0110
#define SHRC    0x07    // 0111
#define SHRA    0x08    // 1000
#define NEG     0x09    // 1001 
#define NOT     0x0A    // 1010
#define AND     0x0B    // 1011
#define OR      0x0C    // 1100 
#define XOR     0x0D    // 1101
#define FSWAP   0x0E    // 1110
#endif