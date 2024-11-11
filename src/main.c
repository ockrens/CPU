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

// Structure to store label locations
typedef struct {
    char name[20];
    int address;
} Label;

Label labels[MAX_LABELS];
int label_count = 0;

// Helper function to convert a string to uppercase
void to_uppercase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
}

// Function to add a label to the labels array
void add_label(const char *name, int address) {
    strcpy(labels[label_count].name, name);
    labels[label_count].address = address;
    label_count++;
}

// Function to find a label's address
int find_label_address(const char *name) {
    for (int i = 0; i < label_count; i++) {
        if (strcmp(labels[i].name, name) == 0) {
            return labels[i].address;
        }
    }
    return -1;  // Label not found
}
int register_to_code(const char *reg) {
    if (strcmp(reg, "R0") == 0) return 0;
    if (strcmp(reg, "R1") == 0) return 1;
    if (strcmp(reg, "R2") == 0) return 2;
    if (strcmp(reg, "R3") == 0) return 3;
    return -1; // Invalid register
}

// Function to convert an opcode to a 4-bit opcode number
int opcode_to_code(const char *opcode) {
    if (strcmp(opcode, "ADD") == 0) return 0;
    if (strcmp(opcode, "SUB") == 0) return 1;
    if (strcmp(opcode, "MUL") == 0) return 2;
    if (strcmp(opcode, "DIV") == 0) return 3;
    return -1; // Invalid opcode
}
// Function to get binary code for an instruction
unsigned char get_instruction_code(const char *instruction, const char *operand1, const char *operand2, int *binary_extra) {
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

int main(int argc, char *argv[]) {
     if (argc != 3) {
        fprintf(stderr, "Usage: %s <input.asm> <output.bin>\n", argv[0]);
        return 1;
    }

    FILE *asm_file = fopen(argv[1], "r");
    if (asm_file == NULL) {
        perror("Error opening input file");
        return 1;
    }

    FILE *bin_file = fopen(argv[2], "wb");
    if (bin_file == NULL) {
        perror("Error opening output file");
        fclose(bin_file);
        return 1;
    }


    // First pass: Collect labels
    char line[LINE_SIZE];
    int address = 0;
    while (fgets(line, sizeof(line), asm_file)) {
        line[strcspn(line, "\n")] = 0;
        to_uppercase(line);

        // Check for label (format: LABEL:)
        char *colon = strchr(line, ':');
        if (colon) {
            *colon = '\0';  // Remove colon
            add_label(line, address);
            continue;  // Skip to the next line
        }

        // Update address for each instruction
        address++;
    }

    // Rewind file for second pass
    rewind(asm_file);

    // Second pass: Convert instructions to binary
    while (fgets(line, sizeof(line), asm_file)) {
        line[strcspn(line, "\n")] = 0;
        to_uppercase(line);

        // Skip labels
        if (strchr(line, ':')) continue;

        // Parse instruction and operands
        char instruction[10], operand1[10], operand2[10];
        int items_read = sscanf(line, "%s %[^,], %s", instruction, operand1, operand2);

        if (items_read < 3) operand2[0] = '\0';

        // Get binary code for the instruction
        int binary_extra;
        int binary_code = get_instruction_code(instruction, operand1, operand2, &binary_extra);

        if (binary_code != 0xFF) {
            // Write the main binary code
            fwrite(&binary_code, sizeof(uint16_t), 1, bin_file);
            if (binary_code == JMP_BASE || binary_code == JEQ_BASE) {
                // Write the address for jump instructions
                fwrite(&binary_extra, sizeof(uint16_t), 1, bin_file);
            }
            printf("Converted '%s' to binary: 0x%04X\n", line, binary_code);
        } else {
            printf("Unknown instruction: %s\n", line);
        }
    }

    fclose(asm_file);
    fclose(bin_file);
    printf("Assembly to binary conversion completed.\n");

    return 0;
}


