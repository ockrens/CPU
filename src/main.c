#include "..\include\main.h"



// .\CPU-Assembler program.asm program.bin

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
        uint16_t binary_code = get_instruction_code(instruction, operand1, operand2, &binary_extra);

        
            // Write the main binary code
            fwrite(&binary_code, sizeof(uint16_t), 1, bin_file);
            printf("Converted '%s' to binary: 0x%04X\n", line, binary_code);
        
    }

    fclose(asm_file);
    fclose(bin_file);
    printf("Assembly to binary conversion completed.\n");

    return 0;
}


