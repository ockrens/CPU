#ifndef remake_h
#define remake_h

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>

#define MemorySize 1000 // for tests
// #define MemorySize 32768 // is half of the memoryspace. the other half will be ram
#define DefaultValue 0x00
// #define InstructionLength sizeof(uint16_t)
#define MemoryWith uint8_t // how many bits the memory is. its defined to make it compatible with other memory types

//Writes the buffer to the output file
void WriteDocument(FILE *file);

#endif