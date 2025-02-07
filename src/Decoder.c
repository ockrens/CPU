/**
 * @file Decoder.c
 * @author Rens J. Ockhuijsen
 * @date 2025-02-07
 * @brief Here are the definitions for how the strings turn in to the binary.
 */


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

uint8_t check_if_reg(const char *reg){
    if (strcmp(reg, "R1") == 0 |strcmp(reg, "R2") == 0 |strcmp(reg, "R3") == 0 |strcmp(reg, "R4") == 0 |strcmp(reg, "R5") == 0 |strcmp(reg, "R6") == 0 |strcmp(reg, "R7") == 0) return 1;
    else return 0;
}


InstructionLength get_instruction_code(const char *instruction, const char *operand1, const char *operand2, int *binary_extra) {
    *binary_extra = 0;  // No extra data by default
    uint16_t instructionOut;

    if (strcmp(instruction, "LDI") == 0) {
        instructionOut = (atoi(operand2)<< 8) | (register_to_code(operand1) << 4) | LDI;            // 8 bits for the immediate value (first 8 bits)
        return instructionOut;
    }


    else if (strcmp(instruction, "JMP") == 0) {
         /* not implemented yet */ if(check_if_reg(operand1)) instructionOut = (register_to_code(operand1)<< 4) | JMP; // if the defined string for operand 1 is between R1-R7 it will jump to the value of the register.
        else instructionOut = (GetLabel(operand1)<< 8) | JMP; // if operand is not a register it will jump using the immediate.
        return instructionOut;
    }


    else if (strcmp(instruction, "JMPR") == 0){

        /* not implemented yet */if(check_if_reg(operand1)) instructionOut = (atoi(operand2)<< 8)|(register_to_code(operand1)<< 4) | JMPR; // if the defined string for operand 1 is between R1-R7 it will jump to the value of the register.
        else instructionOut = ((atoi(operand1) << 8)|JMPR); // if operand is not a register it will jump using the immediate.
        return instructionOut;
    }





    else if (strcmp(instruction, "ADD-example") == 0 && strcmp(operand1, "A") == 0 && strcmp(operand2, "B") == 0) {
        return ADD_A_B;
    }
    else if (strcmp(instruction, "STORE-example") == 0 && strcmp(operand1, "A") == 0) {
        return STORE_A;
    }
    else if (strcmp(instruction, "JEQ-example") == 0) {
        *binary_extra = GetLabel(operand1);
        if (*binary_extra != -1) return JEQ_BASE;
    }
}