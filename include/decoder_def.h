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
#define LDI     0x00    // 0x0000 load immediate value into register 
#define MOV     0x04    // 0x0100 move value from one register to another
#define ALU     0x0C    // 0x1100 ALU operation

// alu operations

/** 
    RIGHT HAND SIDE
 * XX00   ADD
 * XX01   SUB
 * XX10   ADDC
 * XX11   SUBC
 */
/** 
    LEFT HAND SIDE
 * 00XX   ZERO   
 * 01XX   UNCHANGED
 * 10XX   SHL
 * 11XX   SHR
 */
#define ADD         0x4    // 0100  add
#define SUB         0x5    // 0101  subtract
#define ADDC        0x6    // 0110  add with carry
#define SUBC        0x7    // 0111  subtract with carry
#define ZERO        0x0    // 0000
#define UNCHANGED   0x1    // 0100
#define SHL         0x2    // 1000 Make it so it takes a register with the value 0 on the right hand side
#define SHR         0x3    // 1100 Make it so it takes a register with the value 0 on the right hand side

#endif