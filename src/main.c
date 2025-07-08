/**
 * @file main.c
 * @author Rens J. Ockhuijsen
 * @date 2025-02-07
 * @brief This is the file where the gluing happens.
 */

#include "..\include\main.h"


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
    // prints the contents of the buffer
    // for ( int i = 0; i < MemorySize; i++){
    //     printf("value 0x%02X\n", buffer[i]);
    // }
    // fclose(asm_file);
    // fclose(bin_file);
    return 0;
}