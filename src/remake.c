/**
 * @file remake.c
 * @author Rens J. Ockhuijsen
 * @date 2025-02-03
 * @brief main file for the rewrite
 */

#include "..\include\remake.h"
MemoryWith buffer[MemorySize];

void WriteDocument(FILE *file){

    for ( int i = 0; i < MemorySize; i++){
        fwrite(&buffer[i], 1, 1, file);
    }
}

    

int main(int argc, char *argv[]) {
     memset(buffer, DefaultValue, MemorySize);

     // makes sure the correct files are given in the run argument
     if (argc != 3) {
        fprintf(stderr, "Usage: %s <input.asm> <output.bin>\n", argv[0]);
        return 1;
    }

    // opens the files with the given arguments
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
    for ( int i = 0; i < MemorySize; i++){
        printf("value 0x%02X\n", buffer[i]);
    }
    WriteDocument(bin_file);
    fclose(asm_file);
    fclose(bin_file);
    return 0;
}

