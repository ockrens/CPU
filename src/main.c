#include <stdio.h>
#include <stdint.h>  // For standard fixed-width integer types

// Function to write a 16-bit number in little-endian format at a specific file position
void write_little_endian_16_at_offset(FILE *file, uint16_t value, long offset) {
    // Move the file pointer to the specified address
    fseek(file, offset, SEEK_SET);

    // Extract the least significant byte (LSB) and most significant byte (MSB)
    unsigned char lsb = value & 0xFF;       // Least significant byte (low byte)
    unsigned char msb = (value >> 8) & 0xFF; // Most significant byte (high byte)

    // Write the bytes in little-endian order (LSB first, then MSB)
    fwrite(&lsb, sizeof(unsigned char), 1, file);
    fwrite(&msb, sizeof(unsigned char), 1, file);
}

// Function to fill the file with 0xFF up to a specified length
void fill_with_ff(FILE *file, long length) {
    fseek(file, 0, SEEK_SET);  // Move to the beginning of the file
    for (long i = 0; i < length; i++) {
        unsigned char value = 0xFF;
        fwrite(&value, sizeof(unsigned char), 1, file);
    }
}

int main() {
    // Open the .bin file for writing in binary mode
    FILE *file = fopen("output.bin", "wb");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    // Fill the file with 0xFF to a length of 32 bytes (for example)
    fill_with_ff(file, 32);  // Adjust length as needed

    // Write multiple 16-bit values at specific addresses
    write_little_endian_16_at_offset(file, 255, 0);     // Write 255 at address 0
    write_little_endian_16_at_offset(file, 1024, 4);    // Write 1024 at address 4
    write_little_endian_16_at_offset(file, 65535, 10);  // Write 65535 at address 10
    write_little_endian_16_at_offset(file, 42, 16);     // Write 42 at address 16
    write_little_endian_16_at_offset(file, 128, 20);    // Write 128 at address 20

    // Close the file
    fclose(file);

    printf("Multiple values written to output.bin with unused space filled with FF.\n");
    return 0;
}
