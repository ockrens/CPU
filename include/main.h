#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>

// Define binary codes for instructions
#define LOAD_A_5    0x11  // Binary: 0001 0001
#define LOAD_B_10   0x12  // Binary: 0001 0010
#define ADD_A_B     0x21  // Binary: 0010 0001
#define STORE_A     0x31  // Binary: 0011 0001
#define JMP_BASE    0x40  // Base for JMP instructions
#define JEQ_BASE    0x50  // Base for JEQ instructions

// Define maximum labels and line size
#define MAX_LABELS 100
#define LINE_SIZE  50

// Register values
#define R0 0x00
#define R1 0x01
#define R2 0x02
#define R3 0x03
#define R4 0x04
#define R5 0x05
#define R6 0x06
#define R7 0x07

// Function to convert a register name to its corresponding code
uint8_t register_to_code(const char *reg);

// Function to get the binary code for an instruction
uint16_t get_instruction_code(const char *instruction, const char *operand1, const char *operand2, int *binary_extra);

// Function to add a label to the labels array
int find_label_address(const char *name);

#endif