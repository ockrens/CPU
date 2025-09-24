/**
 * @file Functions.c
 * @author Rens J. Ockhuijsen
 * @date 2025-02-07
 * @brief This is where most of the small functions are located to clean the main file.
 */

#include "..\include\main.h"


static Label labels[MAX_LABELS];
int label_count = 0;


void WriteDocument(FILE *file){

    for ( int i = 0; i < MemorySize; i++){
        fwrite(&buffer[i], 1, 1, file);
    }
}



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

    char instruction[10], operand1[10], operand2[10];

    // Second pass: Convert instructions to binary
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        ToUppercase(line);

        // Skip labels
        if (strchr(line, ':')) continue;

        // Parse instruction and operands 
        int items_read = sscanf(line, "%s %[^,], %s", instruction, operand1, operand2);

        if (items_read < 3) operand2[0] = '\0';

        // Get binary code for the instruction
        int binary_extra;
        InstructionLength DecodeResult = get_instruction_code(instruction, operand1, operand2, &binary_extra); // verander instructie naam
        printf("Decode Result: %04X ", DecodeResult);
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
