/**
 * @file decoder_def.h
 * @author Rens J. Ockhuijsen
 * @date 2025-02-07
 * @brief This is where the defenitions for the decoder are.
 */

#ifndef decoder_def_h
#define decoder_def_h

// Register mapping
#define R0      0x00
#define R1      0x01
#define R2      0x02
#define R3      0x03
#define R4      0x04
#define R5      0x05
#define R6      0x06
#define R7      0x07

#define LDI     0x04
#define COUNT   0x00    // adds 2 to the pc                 does not have to have a encoding in the opcode it can be deactiveted on the JMP instructions
#define JMPR    0x01    // jump relative to current state
#define JMP     0x02    // jump based on te lable given     need to make it so it can jump with a immediate

#endif