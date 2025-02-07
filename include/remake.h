#ifndef remake_h
#define remake_h

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>
#include <math.h>

/*-------------------------------------------------------------------------------
dit is een stukje dat overgecopieerd is en moet mogelijk nog veranderingen hebben
-------------------------------------------------------------------------------*/

// Define binary codes for instructions
#define ADD_A_B     0x21  // Binary: 0010 0001
#define STORE_A     0x31  // Binary: 0011 0001
#define JEQ_BASE    0x50  // Base for JEQ instructions

// Register mapping
#define R0      0x00
#define R1      0x01
#define R2      0x02
#define R3      0x03
#define R4      0x04
#define R5      0x05
#define R6      0x06
#define R7      0x07

#define LDI    0x04
#define COUNT   0x00    // adds 2 to the pc                 does not have to have a encoding in the opcode it can be deactiveted on the JMP instructions
#define JMPR    0x01    // jump relative to current state
#define JMP     0x02    // jump based on te lable given     need to make it so it can jump with a immediate
#define ZERO    0x03    // sets the to 0                    does not have to have a encoding in the opcode it only needs to be active when the rest circuit is active !!!!! i can disable the mux to make it zero. way better option becouse its a seperate signal and dont need an encoding for it
 /*-------
    tot hier
    -------*/
#define MAX_LABELS 100
#define LINE_SIZE  50

#define MemorySize 20 // for tests
// #define MemorySize 32768 // is half of the memoryspace. the other half will be ram
#define DefaultValue 0x00
typedef uint16_t InstructionLength;
typedef uint8_t MemoryWith;        // how many bits the memory is. its defined to make it compatible with other memory types


// Structure to store label locations
typedef struct {
    char name[20];
    int address;
} Label;



MemoryWith buffer[MemorySize];

//Writes the buffer to the output file
void WriteDocument(FILE *OutputFile);

//Reads the lables in the input file
void ReadLabels(FILE *InputFile);

//Reads the lines and decodes them to binary
void ProcessFile(FILE *InputFile);

int GetLabel(const char *name);

void AddLabel(const char *name, int address);
void ToUppercase(char *str);

// Function to get the binary code for an instruction
InstructionLength get_instruction_code(const char *instruction, const char *operand1, const char *operand2, int *binary_extra);
#endif



