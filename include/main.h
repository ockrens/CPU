#ifndef MAIN_H
#define MAIN_H

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
#define LOAD    0x0F    // not suported int the cpu at the moment
#define JMP     0x01    // jump based on te lable given
#define JMPA    0x02    // jump to absolute value
#define JMPR    0x03    // jump based on register - maybe it does not have a register it auto detects that it is absolute or relative 
#define JMPL    0x04    // jump relative to current state
#define JMPRL   0xFF    // not implemented
#define ALU     0x0F    // not implemented

// ALU defines
#define ALU_ADD     0x0F    // not implemented
#define ALU_SUB     0x0F    // not implemented


// Function to convert a register name to its corresponding code
uint8_t register_to_code(const char *reg);

// Function to get the binary code for an instruction
uint16_t get_instruction_code(const char *instruction, const char *operand1, const char *operand2, int *binary_extra);

// Function to add a label to the labels array
int find_label_address(const char *name);

#endif