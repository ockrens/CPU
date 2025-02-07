/**
 * @file remake.c
 * @author Rens J. Ockhuijsen
 * @date 2025-02-05
 * @brief main file for the rewrite
 */

#include "..\include\remake.h"
MemoryWith buffer[MemorySize];

Label labels[MAX_LABELS];
int label_count = 0;


void WriteDocument(FILE *file){

    for ( int i = 0; i < MemorySize; i++){
        fwrite(&buffer[i], 1, 1, file);
    }
}

//Makes alle the letters in the string uppercase
void ToUppercase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
}

void AddLabel(const char *name, int address) {
    strcpy(labels[label_count].name, name);
    labels[label_count].address = address * 2;
    label_count++;
}

int GetLabel(const char *name) {
    for (int i = 0; i < label_count; i++) {
        if (strcmp(labels[i].name, name) == 0) {
            return labels[i].address;
        }
    }
    return -1;  // Label not found
}   
/*-------------------------------------------------------------------------------
dit is een stukje dat overgecopieerd is en moet mogelijk nog veranderingen hebben
-------------------------------------------------------------------------------*/
void ReadLabels(FILE *file){
    char line[LINE_SIZE];
    int address = 0; //Starting adress
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0; // Copys the string untill the first punctuation mark
        ToUppercase(line);                 

        // Check for label (format: LABEL:)
        char *colon = strchr(line, ':');
        if (colon) {
            *colon = '\0';  // Remove colon
            AddLabel(line, address);
            continue;  // Skip to the next line
        }

        // Update address for each instruction so it keeps track
        address++;
    }

    // Rewind file so it points to the beginning again
    rewind(file);
}



void ProcessFile(FILE *file){
    InstructionLength DataToBuffer;
    char line[LINE_SIZE];
    int address = 0; //begin address van het document

    MemoryWith FirstPart;
    MemoryWith SecondPart;
    MemoryWith ThirdPart;
    MemoryWith FourthPart;
    // Second pass: Convert instructions to binary
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        ToUppercase(line);

        // Skip labels
        if (strchr(line, ':')) continue;

        // Parse instruction and operands
        char instruction[10], operand1[10], operand2[10];
        int items_read = sscanf(line, "%s %[^,], %s", instruction, operand1, operand2);

        if (items_read < 3) operand2[0] = '\0';

        // Get binary code for the instruction
        int binary_extra;
        InstructionLength DecodeResult = get_instruction_code(instruction, operand1, operand2, &binary_extra); // verander instructie naam
        printf("Decode Result: %0X Mem: %d ", DecodeResult, sizeof(MemoryWith));
        double SizeCheck = sizeof(InstructionLength) / sizeof(MemoryWith);
        if (fmod(SizeCheck, 1.0) == 0.0) {  
           switch ((int)SizeCheck){
            case 1:
                buffer[address] = DecodeResult;
            break;
            case 2:
                FirstPart    = DecodeResult;
                SecondPart   = DecodeResult >> (sizeof(MemoryWith) * 8);
                buffer[address] = FirstPart;
                address++;
                buffer[address] = SecondPart;
                address++;
                printf("1: %02X, 2: %02X\n", FirstPart, SecondPart);
                
            break;
            case 4:
                FirstPart    = DecodeResult;
                SecondPart   = DecodeResult >> (sizeof(MemoryWith) * 8);
                ThirdPart    = DecodeResult >> (sizeof(MemoryWith) * 8 * 2);
                FourthPart   = DecodeResult >> (sizeof(MemoryWith) * 8 * 3);
                buffer[address] = FirstPart;
                address++;
                buffer[address] = SecondPart;
                address++;
                buffer[address] = ThirdPart;
                address++;
                buffer[address] = FourthPart;
                address++;
            break;
           
            default:
                printf("Fault with Writing\n");
            break;
           }
        }
        else {
        printf("%.2f This is not possible\n", SizeCheck);
        }
    }
}
  /*-------
    tot hier
    -------*/

int main(int argc, char *argv[]) {
     memset(buffer, DefaultValue, MemorySize); // writes the default value to the buffer

     // Makes sure the correct files are given in the run argument
     if (argc != 3) {
        fprintf(stderr, "Usage: %s <input.asm> <output.bin>\n", argv[0]);
        return 1;
    }

    // Opens the files with the given arguments
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


    ReadLabels(asm_file);
    ProcessFile(asm_file);
    WriteDocument(bin_file);
//     // prints the contents of the buffer
//     for ( int i = 0; i < MemorySize; i++){
//         printf("value 0x%02X\n", buffer[i]);
//     }
//     fclose(asm_file);
//     fclose(bin_file);
//     return 0;
}

/*-------------------------------------------------------------------------------
dit is een stukje dat overgecopieerd is en moet mogelijk nog veranderingen hebben
-------------------------------------------------------------------------------*/
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


uint16_t get_instruction_code(const char *instruction, const char *operand1, const char *operand2, int *binary_extra) {
    *binary_extra = 0;  // No extra data by default
    uint16_t instructionOut;

    if (strcmp(instruction, "LDI") == 0) {
        instructionOut = (atoi(operand2)<< 8) | (register_to_code(operand1) << 4) | LDI;            // 8 bits for the immediate value (first 8 bits)
        return instructionOut;
    }


    else if (strcmp(instruction, "JMP") == 0) {
         /* not implemented yet */ if(check_if_reg(operand1)) instructionOut = (register_to_code(operand1)<< 4) | JMP; // if the defined string for operand 1 is between R1-R7 it will jump to the value of the register.
        else instructionOut = (GetLabel(operand1)<< 8)|JMP; // if operand is not a register it will jump using the immediate.
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
  /*-------
    tot hier
    -------*/