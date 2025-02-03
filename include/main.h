#ifndef MAIN_H
#define MAIN_H

/* not implemented yet */ // this means its not inplemented in the hardware yet
// .\CPU-Assembler program.asm program.bin

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>

// Define binary codes for instructions
#define ADD_A_B     0x21  // Binary: 0010 0001
#define STORE_A     0x31  // Binary: 0011 0001
#define JEQ_BASE    0x50  // Base for JEQ instructions

// Define maximum labels and line size
#define MAX_LABELS 100
#define LINE_SIZE  50

// Register mapping
#define R0      0x00
#define R1      0x01
#define R2      0x02
#define R3      0x03
#define R4      0x04
#define R5      0x05
#define R6      0x06
#define R7      0x07

// Instruction mapping
/* not implemented yet */#define LDI    0x04

#define COUNT   0x00    // adds 2 to the pc                 does not have to have a encoding in the opcode it can be deactiveted on the JMP instructions
#define JMPR    0x01    // jump relative to current state
#define JMP     0x02    // jump based on te lable given     need to make it so it can jump with a immediate
#define ZERO    0x03    // sets the to 0                    does not have to have a encoding in the opcode it only needs to be active when the rest circuit is active !!!!! i can disable the mux to make it zero. way better option becouse its a seperate signal and dont need an encoding for it
//  00 - count
//  01 - add imm
//  10 - load imm
//  11 - zero


// ALU defines
/* not implemented yet */ #define ALU_ADD     0x0F   
/* not implemented yet */ #define ALU_SUB     0x0F    

// Structure to store label locations
typedef struct {
    char name[20];
    int address;
} Label;

// Function to convert a register name to its corresponding code
uint8_t register_to_code(const char *reg);

// Function to get the binary code for an instruction
uint16_t get_instruction_code(const char *instruction, const char *operand1, const char *operand2, int *binary_extra);

// Function to add a label to the labels array
int find_label_address(const char *name);

//checks if one of the opperands is register 1-7
uint8_t check_if_reg(const char *reg);

#endif