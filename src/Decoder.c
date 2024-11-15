#include "..\include\main.h"

uint8_t register_to_code(const char *reg) {
    if (strcmp(reg, "R0") == 0) return R0;
    if (strcmp(reg, "R1") == 0) return R1;
    if (strcmp(reg, "R2") == 0) return R2;
    if (strcmp(reg, "R3") == 0) return R3;
    if (strcmp(reg, "R4") == 0) return R4;
    if (strcmp(reg, "R5") == 0) return R5;
    if (strcmp(reg, "R6") == 0) return R6;
    if (strcmp(reg, "R7") == 0) return R7;
    return -1; // Invalid register
}

// Function to get binary code for an instruction
uint16_t get_instruction_code(const char *instruction, const char *operand1, const char *operand2, int *binary_extra) {
    *binary_extra = 0;  // No extra data by default
    uint16_t instructionOut;

    if (strcmp(instruction, "LOAD") == 0) {
        instructionOut = (atoi(operand2)<< 8) | (register_to_code(operand1) << 4) | LOAD;            // 8 bits for the immediate value (first 8 bits)
        return instructionOut;
    }

    else if (strcmp(instruction, "JMP") == 0) {     
        uint8_t addr = find_label_address(operand1);
        instructionOut = (addr<< 8) | JMP;
        return instructionOut;
    }

    else if (strcmp(instruction, "JMPR") == 0){
        instructionOut = (register_to_code(operand1) << 4) | JMPR;
        return instructionOut;
    }

    else if (strcmp(instruction, "JMPA") == 0){
        instructionOut = (atoi(operand1)<< 8) | JMPA;
        return instructionOut;
    }
        else if (strcmp(instruction, "JMPL") == 0){
        instructionOut = (atoi(operand1)<< 8) | JMPL;
        return instructionOut;
    }


    
    else if (strcmp(instruction, "ADD-example") == 0 && strcmp(operand1, "A") == 0 && strcmp(operand2, "B") == 0) {
        return ADD_A_B;
    }
    else if (strcmp(instruction, "STORE-example") == 0 && strcmp(operand1, "A") == 0) {
        return STORE_A;
    }
    else if (strcmp(instruction, "JEQ-example") == 0) {
        *binary_extra = find_label_address(operand1);
        if (*binary_extra != -1) return JEQ_BASE;
    }
}