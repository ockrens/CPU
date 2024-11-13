#include "..\include\main.h"

uint8_t register_to_code(const char *reg) {
    if (strcmp(reg, "R0") == 0) return R0;
    if (strcmp(reg, "R1") == 0) return R1;
    if (strcmp(reg, "R2") == 0) return R2;
    if (strcmp(reg, "R3") == 0) return R3;
    return -1; // Invalid register
}

// Function to get binary code for an instruction
uint16_t get_instruction_code(const char *instruction, const char *operand1, const char *operand2, int *binary_extra) {
    *binary_extra = 0;  // No extra data by default
    uint16_t instructionOut;
    uint16_t opcode_code;
    if (strcmp(instruction, "LOAD") == 0) {

        opcode_code = 0001;
        instructionOut = (atoi(operand2)<< 8) | ((register_to_code(operand1)) << 4) | opcode_code;            // 8 bits for the immediate value (first 8 bits)
        
        printf("code %04x\n",instructionOut);

        return instructionOut;
    }
    else if (strcmp(instruction, "ADD") == 0 && strcmp(operand1, "A") == 0 && strcmp(operand2, "B") == 0) {
        return ADD_A_B;
    }
    else if (strcmp(instruction, "STORE") == 0 && strcmp(operand1, "A") == 0) {
        return STORE_A;
    }
    else if (strcmp(instruction, "JMP") == 0) {
        *binary_extra = find_label_address(operand1);
        if (*binary_extra != -1) return JMP_BASE;
    }
    else if (strcmp(instruction, "JEQ") == 0) {
        *binary_extra = find_label_address(operand1);
        if (*binary_extra != -1) return JEQ_BASE;
    }
}