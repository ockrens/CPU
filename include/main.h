/**
 * @file main.h
 * @author Rens J. Ockhuijsen
 * @date 2025-02-07
 * @brief This is where most things are defined.
 */

#ifndef main_h
#define main_h

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>
#include <math.h>
#include "..\include\decoder_def.h"

/*-------------------------------------------------------------------------
----------------------- Memory settings -----------------------------------
-------------------------------------------------------------------------*/

// The value that every unused address should be (like a HLT).
#define DefaultValue 0x00

// How many bits the instuctions set is.
typedef uint16_t InstructionLength;

// How many bits the memory is. its defined to make it compatible with other memory types.
typedef uint8_t MemoryWith;        

/*-------------------------------------------------------------------------
----------------------- Structs and variables -----------------------------
-------------------------------------------------------------------------*/

// Structure to store label locations
typedef struct {
    char name[20];
    int address;
} Label;

// The ammount af addresses the memory has.
#define MemorySize 40

// Makes the buffer
MemoryWith buffer[MemorySize];

/*-------------------------------------------------------------------------
----------------------- Function defenitions ------------------------------
-------------------------------------------------------------------------*/

// Writes the buffer to the output file.
void WriteDocument(FILE *OutputFile);

// Reads the lables in the input file.
void ReadLabels(FILE *InputFile);

// Reads the lines and decodes them to binary.
void ProcessFile(FILE *InputFile);

// Gets the address for a given label.
int GetLabel(const char *name);

// Binds a adress to a given label name.
void AddLabel(const char *name, int address);

// Coverts the string to have all uppercase. this is so you dont need to type the assembly in uppercase if you want.
void ToUppercase(char *str);

// Function to get the binary code for a given nstruction.
InstructionLength get_instruction_code(const char *instruction, const char *operand1, const char *operand2, int *binary_extra);


#endif

/*-------------------------------------------------------------------------
----------------------- Temporary defenitions -----------------------------
-------------------------------------------------------------------------*/

// Define binary codes for instructions
#define ADD_A_B     0x21  // Binary: 0010 0001
#define STORE_A     0x31  // Binary: 0011 0001
#define JEQ_BASE    0x50  // Base for JEQ instructions

// Register mapping
#define MAX_LABELS 100
#define LINE_SIZE  50


// #define MemorySize 32768 // is half of the memoryspace. the other half will be ram



